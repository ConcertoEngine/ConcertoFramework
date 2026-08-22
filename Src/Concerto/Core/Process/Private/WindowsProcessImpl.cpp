#include "Concerto/Core/Defines.hpp"

#ifdef CCT_PLATFORM_WINDOWS

#include <windows.h>

#include "Concerto/Core/Process/Process.hpp"

namespace cct
{
	namespace
	{
		constexpr UInt32 NtStatusSeverityShift = 30;
		constexpr UInt32 NtStatusSeverityError = 3;
	} // namespace

	std::filesystem::path Process::GetCurrentExecutablePath()
	{
		wchar_t buffer[MAX_PATH] = {};
		const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);

		return std::filesystem::path(buffer, buffer + length);
	}

	ProcessResult Process::Run(const std::filesystem::path& executable,
							   const std::vector<std::string>& arguments,
							   std::chrono::milliseconds timeout)
	{
		std::string commandLine = "\"" + executable.string() + "\"";
		for (const std::string& argument : arguments)
			commandLine += " \"" + argument + "\"";

		STARTUPINFOA startupInfo = {};
		startupInfo.cb = sizeof(startupInfo);

		PROCESS_INFORMATION processInfo = {};

		if (!CreateProcessA(nullptr, commandLine.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startupInfo, &processInfo))
			return {ProcessOutcome::SpawnFailed, 0, static_cast<UInt32>(GetLastError())};

		ProcessResult result;

		if (WaitForSingleObject(processInfo.hProcess, static_cast<DWORD>(timeout.count())) == WAIT_TIMEOUT)
		{
			TerminateProcess(processInfo.hProcess, 1);
			WaitForSingleObject(processInfo.hProcess, INFINITE);
			result.outcome = ProcessOutcome::TimedOut;
		}
		else
		{
			DWORD exitCode = 0;
			GetExitCodeProcess(processInfo.hProcess, &exitCode);

			if ((exitCode >> NtStatusSeverityShift) == NtStatusSeverityError)
			{
				result.outcome = ProcessOutcome::Crashed;
				result.faultCode = exitCode;
			}
			else
			{
				result.outcome = ProcessOutcome::Exited;
				result.exitCode = static_cast<Int32>(exitCode);
			}
		}

		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);

		return result;
	}
} // namespace cct

#endif
