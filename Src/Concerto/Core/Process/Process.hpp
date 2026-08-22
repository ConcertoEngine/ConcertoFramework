#ifndef CONCERTO_CORE_PROCESS_HPP
#define CONCERTO_CORE_PROCESS_HPP

#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	enum class ProcessOutcome : UInt8
	{
		Exited,
		Crashed,
		TimedOut,
		SpawnFailed,
	};

	struct ProcessResult
	{
		ProcessOutcome outcome = ProcessOutcome::SpawnFailed;
		Int32 exitCode = 0;

		// Windows exception code, or POSIX signal number.
		UInt32 faultCode = 0;
	};

	[[nodiscard]] CCT_CORE_PUBLIC_API std::string_view ToString(ProcessOutcome outcome) noexcept;

	class CCT_CORE_PUBLIC_API Process
	{
	public:
		[[nodiscard]] static std::filesystem::path GetCurrentExecutablePath();

		// Runs a child to completion, killing it if it outlives the timeout.
		[[nodiscard]] static ProcessResult Run(const std::filesystem::path& executable,
											   const std::vector<std::string>& arguments,
											   std::chrono::milliseconds timeout);
	};
} // namespace cct

#endif // CONCERTO_CORE_PROCESS_HPP
