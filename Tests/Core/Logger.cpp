/**
 * @file Tests/Logger.cpp
 * @brief Unit tests for Logger, categories and channels
 * @date 2026-04-10
 */

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#include <Concerto/Core/Logger/Logger.hpp>
#include <Concerto/Core/Logger/LogMacros.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace cct;

namespace CCT_ANONYMOUS_NAMESPACE
{
	// Creates a file-backed Logger configuration in a unique temp directory.
	// Returns both the config and the path to the produced log file so tests
	// can flush and re-read the contents.
	struct TempFileLoggerGuard
	{
		std::filesystem::path Dir;
		std::filesystem::path File;

		std::unique_ptr<cct::Logger> m_Logger;

		explicit TempFileLoggerGuard(const std::string& suffix)
		{
			Dir = std::filesystem::temp_directory_path() / ("cct_logger_test_" + suffix);
			File = Dir / "concerto.log";
			std::error_code ec;
			std::filesystem::remove_all(Dir, ec);
			std::filesystem::create_directories(Dir, ec);

			LogConfig config;
			config.GlobalLevel = LogLevel::Trace;
			config.EnableConsole = false;
			config.EnableFile = true;
			config.LogDir = Dir;
			config.LogFileName = "concerto.log";
			config.Pattern = "[%l] %v";

			m_Logger = std::make_unique<cct::Logger>(config);
			cct::Logger::SetContext(m_Logger.get());
		}

		~TempFileLoggerGuard()
		{
			cct::Logger::SetContext(nullptr);
			m_Logger.reset();
			std::error_code ec;
			std::filesystem::remove_all(Dir, ec);
		}

		TempFileLoggerGuard(const TempFileLoggerGuard&) = delete;
		TempFileLoggerGuard& operator=(const TempFileLoggerGuard&) = delete;

		std::string ReadAll() const
		{
			m_Logger->Flush();
			std::ifstream in(File, std::ios::binary);
			std::stringstream ss;
			ss << in.rdbuf();
			return ss.str();
		}
	};

	SCENARIO("Logger - Instantiation is safe to call across pointers")
	{
		GIVEN("A context pointer config")
		{
			LogConfig cfg;
			cct::Logger logger(cfg);
			cct::Logger::SetContext(&logger);

			THEN("No crash occurs and logging is usable")
			{
				cct::Logger::Info("sanity {}", 1);
				CCT_LOG_ERROR("Core", "Vulkan", "Error in Vulkan");
				CCT_LOG_INFO("Core", "Vulkan", "Info in Vulkan");
				cct::Logger::GetContext()->Flush();
			}

			cct::Logger::SetContext(nullptr);
		}
	}

	SCENARIO("Logger - CCT_LOG_INFO writes through category logger")
	{
		GIVEN("A file-backed logger and a captured Renderer category")
		{
			TempFileLoggerGuard guard("info");
			Logger::GetContext()->SetCategoryLevel("Renderer", LogLevel::Trace);

			WHEN("Logging an info message through the Vulkan channel")
			{
				CCT_LOG_INFO("Renderer", "Vulkan", "Frame {}", 42);
				const auto out = guard.ReadAll();

				THEN("The output contains the channel tag and message")
				{
					CHECK(out.find("[Vulkan]") != std::string::npos);
					CHECK(out.find("Frame 42") != std::string::npos);
					CHECK(out.find("info") != std::string::npos);
				}
			}
		}
	}

	SCENARIO("Logger - Per-category level filtering")
	{
		GIVEN("The Network category set to Warning level")
		{
			TempFileLoggerGuard guard("cat");
			Logger::GetContext()->SetCategoryLevel("Network", LogLevel::Warning);

			WHEN("Logging at Info level")
			{
				CCT_LOG_INFO("Network", "HTTP", "ignored info");
				THEN("Message is suppressed")
				{
					CHECK(guard.ReadAll().find("ignored info") == std::string::npos);
				}
			}

			WHEN("Logging at Error level")
			{
				CCT_LOG_ERROR("Network", "HTTP", "visible error");
				THEN("Message is emitted")
				{
					CHECK(guard.ReadAll().find("visible error") != std::string::npos);
				}
			}
		}
	}

	SCENARIO("Logger - Per-channel filtering")
	{
		GIVEN("A disabled channel on the Audio category")
		{
			TempFileLoggerGuard guard("chan_disable");
			Logger::GetContext()->SetCategoryLevel("Audio", LogLevel::Trace);
			Logger::GetContext()->SetChannelEnabled("Muted", false);

			WHEN("Logging through the disabled channel")
			{
				CCT_LOG_INFO("Audio", "Muted", "should not appear");
				THEN("Message is suppressed")
				{
					CHECK(guard.ReadAll().find("should not appear") == std::string::npos);
				}
			}

			WHEN("Re-enabling the channel and logging")
			{
				Logger::GetContext()->SetChannelEnabled("Muted", true);
				CCT_LOG_INFO("Audio", "Muted", "now visible");
				THEN("Message is emitted")
				{
					CHECK(guard.ReadAll().find("now visible") != std::string::npos);
				}
			}
		}

		GIVEN("A channel with a Warning level filter")
		{
			TempFileLoggerGuard guard("chan_level");
			Logger::GetContext()->SetCategoryLevel("Audio", LogLevel::Trace);
			Logger::GetContext()->SetChannelEnabled("NoisyChan", true);
			Logger::GetContext()->SetChannelLevel("NoisyChan", LogLevel::Warning);

			WHEN("Logging Info and Warning messages")
			{
				CCT_LOG_INFO("Audio", "NoisyChan", "chan-info");
				CCT_LOG_WARN("Audio", "NoisyChan", "chan-warn");
				const auto out = guard.ReadAll();

				THEN("Only the warning is emitted")
				{
					CHECK(out.find("chan-info") == std::string::npos);
					CHECK(out.find("chan-warn") != std::string::npos);
				}
			}
		}
	}

	SCENARIO("Logger - ShouldLog fast path")
	{
		GIVEN("A global level of Warning")
		{
			TempFileLoggerGuard guard("fastpath");
			Logger::GetContext()->SetGlobalLevel(LogLevel::Warning);
			THEN("Info messages are rejected")
			{
				CHECK_FALSE(Logger::GetContext()->ShouldLog("X", "Y", LogLevel::Info));
			}
			THEN("Warning messages are accepted")
			{
				CHECK(Logger::GetContext()->ShouldLog("X", "Y", LogLevel::Warning));
			}
			THEN("Error messages are accepted")
			{
				CHECK(Logger::GetContext()->ShouldLog("X", "Y", LogLevel::Error));
			}
		}
	}

	SCENARIO("Logger - Backward compatibility API still works")
	{
		GIVEN("A file-backed logger")
		{
			TempFileLoggerGuard guard("legacy");
			Logger::GetContext()->SetCategoryLevel("Core", LogLevel::Trace);

			WHEN("Calling Logger::Info")
			{
				Logger::Info("legacy info {}", 7);
				THEN("Message is emitted")
				{
					CHECK(guard.ReadAll().find("legacy info 7") != std::string::npos);
				}
			}

			WHEN("Calling Logger::Warning")
			{
				Logger::Warning("legacy warn {}", 8);
				THEN("Message is emitted")
				{
					CHECK(guard.ReadAll().find("legacy warn 8") != std::string::npos);
				}
			}

			WHEN("Calling Logger::Error")
			{
				Logger::Error("legacy error {}", 9);
				THEN("Message is emitted")
				{
					CHECK(guard.ReadAll().find("legacy error 9") != std::string::npos);
				}
			}
		}
	}

	SCENARIO("Logger - Thread safety smoke test")
	{
		GIVEN("Multiple threads logging concurrently")
		{
			TempFileLoggerGuard guard("threads");
			Logger::GetContext()->SetCategoryLevel("Threaded", LogLevel::Trace);

			constexpr int kThreadCount = 8;
			constexpr int kMessagesPerThread = 50;

			std::vector<std::thread> threads;
			threads.reserve(kThreadCount);
			std::atomic<int> startGate{0};

			for (int t = 0; t < kThreadCount; ++t)
			{
				threads.emplace_back([t, &startGate]()
									 {
					startGate.fetch_add(1, std::memory_order_relaxed);
					while (startGate.load(std::memory_order_relaxed) < kThreadCount) { /* spin */ }
					for (int i = 0; i < kMessagesPerThread; ++i)
					{
						CCT_LOG_INFO("Threaded", "Worker", "t{} msg{}", t, i);
					} });
			}

			for (auto& th : threads)
				th.join();

			const auto out = guard.ReadAll();

			THEN("All messages are recorded without corruption")
			{
				const auto count = std::count(out.begin(), out.end(), '\n');
				CHECK(count == kThreadCount * kMessagesPerThread);
			}
		}
	}

	SCENARIO("Logger - Scoped logging helper")
	{
		GIVEN("A CCT_LOG_SCOPE in a block")
		{
			TempFileLoggerGuard guard("scope");
			Logger::GetContext()->SetCategoryLevel("Profile", LogLevel::Trace);

			{
				CCT_LOG_SCOPE("Profile", "Section", "DoWork");
			}

			const auto out = guard.ReadAll();

			THEN("Enter and Exit markers are logged")
			{
				CHECK(out.find("Enter: DoWork") != std::string::npos);
				CHECK(out.find("Exit: DoWork") != std::string::npos);
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
