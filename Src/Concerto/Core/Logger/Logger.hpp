#ifndef CONCERTO_CORE_LOGGER_HPP
#define CONCERTO_CORE_LOGGER_HPP

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>

#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	enum class LogLevel : std::uint8_t
	{
		Trace = 0,
		Debug = 1,
		Info = 2,
		Warning = 3,
		Error = 4,
		Critical = 5,
		Off = 6
	};

	struct LogConfig
	{
		LogLevel GlobalLevel = LogLevel::Info;
		bool EnableConsole = true;
		bool EnableFile = false;
		std::filesystem::path LogDir = "Logs";
		std::string LogFileName = "concerto.log";
		std::size_t MaxFileSize = 5 * 1024 * 1024; // 5 MB
		std::size_t MaxFiles = 3;
		bool AsyncMode = false;
		std::size_t AsyncQueueSize = 8192;
		std::size_t AsyncThreadCount = 1;
		// NOTE: Pattern uses spdlog format specifiers (%e, %n, %^, %$).
		std::string Pattern = "[%Y-%m-%d %H:%M:%S.%e] [%n] %^[%l] %v%$";
	};

	class CCT_CORE_PUBLIC_API Logger
	{
	public:
		explicit Logger(const LogConfig& config = {});
		~Logger();

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator=(Logger&&) = delete;

		static Logger* GetContext();
		static void SetContext(Logger* context);

		void Flush();
		void SetGlobalLevel(LogLevel level);
		void SetCategoryLevel(std::string_view category, LogLevel level);
		void SetChannelLevel(std::string_view channel, LogLevel level);
		void SetChannelEnabled(std::string_view channel, bool enabled);

		bool ShouldLog(const char* category, const char* channel, LogLevel level) const noexcept;
		void LogMessage(const char* category, const char* channel, LogLevel level, std::string_view message);

		template<typename... T>
		struct Debug
		{
			explicit Debug(const std::format_string<T...> fmt,
						   T&&... args,
						   std::source_location loc = std::source_location::current())
			{
				auto* ctx = Logger::GetContext();
				if (!ctx || !ctx->ShouldLog("Core", "Default", LogLevel::Debug))
					return;
				auto msg = std::format("{}:{}: ", loc.file_name(), loc.line());
				msg += std::vformat(fmt.get(), std::make_format_args(args...));
				ctx->LogMessage("Core", "Default", LogLevel::Debug, msg);
#ifdef CCT_PLATFORM_WINDOWS
				Logger::DebugString(msg);
#endif
			}
		};

		template<typename... Types>
		Debug(std::format_string<Types...>, Types&&...) -> Debug<Types...>;

		template<typename... Types>
		static void Info(const std::format_string<Types...> fmt, Types&&... args)
		{
			auto* ctx = Logger::GetContext();
			if (!ctx || !ctx->ShouldLog("Core", "Default", LogLevel::Info))
				return;
			const auto msg = std::vformat(fmt.get(), std::make_format_args(args...));
			ctx->LogMessage("Core", "Default", LogLevel::Info, msg);
		}

		template<typename... Types>
		static void Warning(const std::format_string<Types...> fmt, Types&&... args)
		{
			auto* ctx = Logger::GetContext();
			if (!ctx || !ctx->ShouldLog("Core", "Default", LogLevel::Warning))
				return;
			const auto msg = std::vformat(fmt.get(), std::make_format_args(args...));
			ctx->LogMessage("Core", "Default", LogLevel::Warning, msg);
		}

		template<typename... Types>
		static void Error(const std::format_string<Types...> fmt, Types&&... args)
		{
			auto* ctx = Logger::GetContext();
			if (!ctx || !ctx->ShouldLog("Core", "Default", LogLevel::Error))
				return;
			const auto msg = std::vformat(fmt.get(), std::make_format_args(args...));
			ctx->LogMessage("Core", "Default", LogLevel::Error, msg);
		}

		static void DebugString(const std::string& string);

	public: // Intentionally public: anonymous-namespace helpers in Logger.cpp
			// need access to LoggerState. The type is incomplete for consumers.
		struct LoggerState;

	private:
		std::unique_ptr<LoggerState> m_State;
	};
} // namespace cct

#endif // CONCERTO_CORE_LOGGER_HPP
