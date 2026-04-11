#ifndef CONCERTO_CORE_LOGMACROS_HPP
#define CONCERTO_CORE_LOGMACROS_HPP

#include <chrono>
#include <format>
#include <source_location>
#include <string>
#include <string_view>

#include "Concerto/Core/Logger/Logger.hpp"

#ifndef CCT_LOG_LEVEL_MINIMUM
#define CCT_LOG_LEVEL_MINIMUM 0
#endif

#define CCT_LOG_IMPL(cctLevel, category, channel, fmtStr, ...)                       \
	do                                                                               \
	{                                                                                \
		if (auto* ctx = ::cct::Logger::GetContext();                                 \
			ctx && ctx->ShouldLog((category), (channel), ::cct::LogLevel::cctLevel)) \
		{                                                                            \
			ctx->LogMessage(                                                         \
				(category),                                                          \
				(channel),                                                           \
				::cct::LogLevel::cctLevel,                                           \
				::std::format(fmtStr __VA_OPT__(, ) __VA_ARGS__));                   \
		}                                                                            \
	} while (false)

#if CCT_LOG_LEVEL_MINIMUM <= 0
#define CCT_LOG_TRACE(category, channel, fmt, ...) \
	CCT_LOG_IMPL(Trace, category, channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#else
#define CCT_LOG_TRACE(category, channel, fmt, ...) \
	do                                             \
	{                                              \
	} while (false)
#endif

#if CCT_LOG_LEVEL_MINIMUM <= 1
#define CCT_LOG_DEBUG(category, channel, fmt, ...)                                \
	do                                                                            \
	{                                                                             \
		if (auto* ctx = ::cct::Logger::GetContext();                              \
			ctx && ctx->ShouldLog((category), (channel), ::cct::LogLevel::Debug)) \
		{                                                                         \
			auto loc = ::std::source_location::current();                         \
			ctx->LogMessage(                                                      \
				(category),                                                       \
				(channel),                                                        \
				::cct::LogLevel::Debug,                                           \
				::std::format("{}:{}:{}: {}",                                     \
							  loc.file_name(),                                    \
							  loc.line(),                                         \
							  loc.column(),                                       \
							  ::std::format(fmt __VA_OPT__(, ) __VA_ARGS__)));    \
		}                                                                         \
	} while (false)
#else
#define CCT_LOG_DEBUG(category, channel, fmt, ...) \
	do                                             \
	{                                              \
	} while (false)
#endif

#if CCT_LOG_LEVEL_MINIMUM <= 2
#define CCT_LOG_INFO(category, channel, fmt, ...) \
	CCT_LOG_IMPL(Info, category, channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#else
#define CCT_LOG_INFO(category, channel, fmt, ...) \
	do                                            \
	{                                             \
	} while (false)
#endif

#if CCT_LOG_LEVEL_MINIMUM <= 3
#define CCT_LOG_WARN(category, channel, fmt, ...) \
	CCT_LOG_IMPL(Warning, category, channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#else
#define CCT_LOG_WARN(category, channel, fmt, ...) \
	do                                            \
	{                                             \
	} while (false)
#endif

#if CCT_LOG_LEVEL_MINIMUM <= 4
#define CCT_LOG_ERROR(category, channel, fmt, ...) \
	CCT_LOG_IMPL(Error, category, channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#else
#define CCT_LOG_ERROR(category, channel, fmt, ...) \
	do                                             \
	{                                              \
	} while (false)
#endif

#if CCT_LOG_LEVEL_MINIMUM <= 5
#define CCT_LOG_CRITICAL(category, channel, fmt, ...) \
	CCT_LOG_IMPL(Critical, category, channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#else
#define CCT_LOG_CRITICAL(category, channel, fmt, ...) \
	do                                                \
	{                                                 \
	} while (false)
#endif

namespace cct::detail
{
	class LogScope
	{
	public:
		LogScope(const char* category, const char* channel, std::string_view name) :
			m_category(category),
			m_channel(channel),
			m_name(name),
			m_start(std::chrono::steady_clock::now())
		{
			if (auto* ctx = Logger::GetContext(); ctx && ctx->ShouldLog(m_category, m_channel, LogLevel::Trace))
			{
				ctx->LogMessage(
					m_category, m_channel, LogLevel::Trace, std::format("Enter: {}", m_name));
			}
		}

		~LogScope()
		{
			if (auto* ctx = Logger::GetContext(); ctx && ctx->ShouldLog(m_category, m_channel, LogLevel::Trace))
			{
				const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::steady_clock::now() - m_start);
				ctx->LogMessage(m_category,
								m_channel,
								LogLevel::Trace,
								std::format("Exit: {} ({} us)", m_name, elapsed.count()));
			}
		}

		LogScope(const LogScope&) = delete;
		LogScope& operator=(const LogScope&) = delete;
		LogScope(LogScope&&) = delete;
		LogScope& operator=(LogScope&&) = delete;

	private:
		const char* m_category;
		const char* m_channel;
		std::string m_name;
		std::chrono::steady_clock::time_point m_start;
	};
} // namespace cct::detail

#define CCT_LOG_SCOPE_CAT(a, b) CCT_LOG_SCOPE_CAT_IMPL(a, b)
#define CCT_LOG_SCOPE_CAT_IMPL(a, b) a##b

#define CCT_LOG_SCOPE(category, channel, name) \
	::cct::detail::LogScope CCT_LOG_SCOPE_CAT(cctLogScope_, __LINE__)((category), (channel), (name))

#endif // CONCERTO_CORE_LOGMACROS_HPP
