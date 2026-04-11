#include "Concerto/Core/Logger/Logger.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#ifdef CCT_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace cct
{
	namespace
	{
		struct ChannelState
		{
			LogLevel Level = LogLevel::Trace;
			bool Enabled = true;
		};

		struct StringHash
		{
			using is_transparent = void;

			std::size_t operator()(std::string_view sv) const noexcept
			{
				return std::hash<std::string_view>{}(sv);
			}
			std::size_t operator()(const std::string& s) const noexcept
			{
				return std::hash<std::string_view>{}(s);
			}
			std::size_t operator()(const char* s) const noexcept
			{
				return std::hash<std::string_view>{}(s);
			}
		};

		constexpr const char* kDefaultCategory = "Core";

		constexpr spdlog::level::level_enum ToSpdlogLevel(LogLevel level) noexcept
		{
			switch (level)
			{
				case LogLevel::Trace:
					return spdlog::level::trace;
				case LogLevel::Debug:
					return spdlog::level::debug;
				case LogLevel::Info:
					return spdlog::level::info;
				case LogLevel::Warning:
					return spdlog::level::warn;
				case LogLevel::Error:
					return spdlog::level::err;
				case LogLevel::Critical:
					return spdlog::level::critical;
				case LogLevel::Off:
					return spdlog::level::off;
			}
			return spdlog::level::info;
		}

		std::shared_ptr<spdlog::logger> CreateLoggerLocked(Logger::LoggerState& state, std::string_view categoryName);
		void InitializeLocked(Logger::LoggerState& state, const LogConfig& config);
	} // namespace

	struct Logger::LoggerState
	{
		mutable std::shared_mutex Mutex;
		std::unordered_map<std::string, std::shared_ptr<spdlog::logger>, StringHash, std::equal_to<>> Loggers;
		std::unordered_map<std::string, ChannelState, StringHash, std::equal_to<>> ChannelStates;
		std::vector<spdlog::sink_ptr> Sinks;
		LogConfig Config;
		std::atomic<LogLevel> FastPathGlobalLevel{LogLevel::Info};
		std::atomic<bool> Initialized{false};
		bool AsyncEnabled = false;
	};

	namespace
	{
		std::shared_ptr<spdlog::logger> CreateLoggerLocked(Logger::LoggerState& state, std::string_view categoryName)
		{
			const std::string name(categoryName);
			std::shared_ptr<spdlog::logger> logger;

			if (state.AsyncEnabled)
			{
				logger = std::make_shared<spdlog::async_logger>(
					name,
					state.Sinks.begin(),
					state.Sinks.end(),
					spdlog::thread_pool(),
					spdlog::async_overflow_policy::block);
			}
			else
			{
				logger = std::make_shared<spdlog::logger>(name, state.Sinks.begin(), state.Sinks.end());
			}

			logger->set_level(ToSpdlogLevel(state.Config.GlobalLevel));
			logger->flush_on(spdlog::level::err);
			return logger;
		}

		void InitializeLocked(Logger::LoggerState& state, const LogConfig& config)
		{
			if (state.Initialized.load(std::memory_order_acquire))
				return;

			state.Config = config;
			state.FastPathGlobalLevel.store(config.GlobalLevel, std::memory_order_relaxed);
			state.Sinks.clear();

			if (config.EnableConsole)
			{
				auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
				consoleSink->set_pattern(config.Pattern);
				state.Sinks.push_back(std::move(consoleSink));
			}

			if (config.EnableFile)
			{
				std::error_code ec;
				std::filesystem::create_directories(config.LogDir, ec);

				if (!ec)
				{
					const auto filePath = (config.LogDir / config.LogFileName).string();

					auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
						filePath, config.MaxFileSize, config.MaxFiles);
					fileSink->set_pattern(config.Pattern);
					state.Sinks.push_back(std::move(fileSink));
				}
			}

			if (config.AsyncMode)
			{
				spdlog::init_thread_pool(config.AsyncQueueSize, config.AsyncThreadCount);
				state.AsyncEnabled = true;
			}
			else
			{
				state.AsyncEnabled = false;
			}

			spdlog::set_level(ToSpdlogLevel(config.GlobalLevel));
			state.Initialized.store(true, std::memory_order_release);
		}

		spdlog::logger* GetOrCreateLoggerInternal(Logger::LoggerState& state, const char* category)
		{
			const char* name = (category != nullptr && *category != '\0') ? category : kDefaultCategory;

			{
				std::shared_lock lock(state.Mutex);
				auto it = state.Loggers.find(name);
				if (it != state.Loggers.end())
					return it->second.get();
			}

			std::unique_lock lock(state.Mutex);
			auto it = state.Loggers.find(name);
			if (it != state.Loggers.end())
				return it->second.get();

			auto logger = CreateLoggerLocked(state, name);
			auto* raw = logger.get();
			state.Loggers.emplace(std::string(name), std::move(logger));
			return raw;
		}
	} // namespace

	static std::atomic<Logger*> s_Context{nullptr};

	Logger* Logger::GetContext()
	{
		return s_Context.load(std::memory_order_acquire);
	}

	void Logger::SetContext(Logger* context)
	{
		s_Context.store(context, std::memory_order_release);
	}

	Logger::Logger(const LogConfig& config) :
		m_State(std::make_unique<LoggerState>())
	{
		std::unique_lock lock(m_State->Mutex);
		InitializeLocked(*m_State, config);
	}

	Logger::~Logger()
	{
		std::unique_lock lock(m_State->Mutex);
		for (auto& [_, logger] : m_State->Loggers)
		{
			if (logger)
				logger->flush();
		}
		m_State->Loggers.clear();
		m_State->ChannelStates.clear();
		m_State->Sinks.clear();
		m_State->Initialized.store(false, std::memory_order_release);
		m_State->AsyncEnabled = false;
	}

	void Logger::Flush()
	{
		std::shared_lock lock(m_State->Mutex);
		for (auto& [_, logger] : m_State->Loggers)
		{
			if (logger)
				logger->flush();
		}
	}

	void Logger::SetGlobalLevel(LogLevel level)
	{
		std::unique_lock lock(m_State->Mutex);
		m_State->Config.GlobalLevel = level;
		m_State->FastPathGlobalLevel.store(level, std::memory_order_relaxed);
		const auto spdLevel = ToSpdlogLevel(level);
		for (auto& [_, logger] : m_State->Loggers)
		{
			if (logger)
				logger->set_level(spdLevel);
		}
	}

	void Logger::SetCategoryLevel(std::string_view category, LogLevel level)
	{
		std::unique_lock lock(m_State->Mutex);
		auto it = m_State->Loggers.find(category);
		if (it == m_State->Loggers.end())
		{
			auto logger = CreateLoggerLocked(*m_State, category);
			logger->set_level(ToSpdlogLevel(level));
			m_State->Loggers.emplace(std::string(category), std::move(logger));
			return;
		}
		if (it->second)
			it->second->set_level(ToSpdlogLevel(level));
	}

	void Logger::SetChannelLevel(std::string_view channel, LogLevel level)
	{
		std::unique_lock lock(m_State->Mutex);
		auto it = m_State->ChannelStates.find(channel);
		if (it == m_State->ChannelStates.end())
		{
			ChannelState s;
			s.Level = level;
			m_State->ChannelStates.emplace(std::string(channel), s);
		}
		else
		{
			it->second.Level = level;
		}
	}

	void Logger::SetChannelEnabled(std::string_view channel, bool enabled)
	{
		std::unique_lock lock(m_State->Mutex);
		auto it = m_State->ChannelStates.find(channel);
		if (it == m_State->ChannelStates.end())
		{
			ChannelState s;
			s.Enabled = enabled;
			m_State->ChannelStates.emplace(std::string(channel), s);
		}
		else
		{
			it->second.Enabled = enabled;
		}
	}

	bool Logger::ShouldLog(const char* category, const char* channel, LogLevel level) const noexcept
	{
		if (static_cast<int>(level) < static_cast<int>(m_State->FastPathGlobalLevel.load(std::memory_order_relaxed)))
			return false;

		try
		{
			std::shared_lock lock(m_State->Mutex);

			if (category != nullptr)
			{
				auto it = m_State->Loggers.find(category);
				if (it != m_State->Loggers.end() && it->second)
				{
					if (!it->second->should_log(ToSpdlogLevel(level)))
						return false;
				}
			}

			if (channel != nullptr)
			{
				auto it = m_State->ChannelStates.find(channel);
				if (it != m_State->ChannelStates.end())
				{
					if (!it->second.Enabled)
						return false;
					if (static_cast<int>(level) < static_cast<int>(it->second.Level))
						return false;
				}
			}
		}
		catch (...)
		{
			return false;
		}
		return true;
	}

	void Logger::LogMessage(const char* category,
							const char* channel,
							LogLevel level,
							std::string_view message)
	{
		auto* logger = GetOrCreateLoggerInternal(*m_State, category);
		if (logger == nullptr)
			return;

		const char* chanName = (channel != nullptr) ? channel : "";
		logger->log(ToSpdlogLevel(level), "[{}] {}", chanName, message);
	}

	void Logger::DebugString(const std::string& string)
	{
#ifdef CCT_PLATFORM_WINDOWS
		OutputDebugStringA(string.c_str());
#else
		(void)string;
#endif
	}
} // namespace cct
