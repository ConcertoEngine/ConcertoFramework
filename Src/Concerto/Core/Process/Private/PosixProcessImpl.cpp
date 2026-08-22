#include "Concerto/Core/Defines.hpp"

#ifdef CCT_PLATFORM_POSIX

#include <csignal>
#include <thread>
#include <unistd.h>

#include "Concerto/Core/Process/Process.hpp"
#include <sys/wait.h>

namespace cct
{
	std::filesystem::path Process::GetCurrentExecutablePath()
	{
#ifdef CCT_PLATFORM_MACOS
		return std::filesystem::canonical("/proc/curproc/file");
#else
		return std::filesystem::canonical("/proc/self/exe");
#endif
	}

	ProcessResult Process::Run(const std::filesystem::path& executable,
							   const std::vector<std::string>& arguments,
							   std::chrono::milliseconds timeout)
	{
		std::vector<std::string> storage;
		storage.reserve(arguments.size() + 1);
		storage.push_back(executable.string());
		storage.insert(storage.end(), arguments.begin(), arguments.end());

		std::vector<char*> argv;
		argv.reserve(storage.size() + 1);
		for (std::string& entry : storage)
			argv.push_back(entry.data());
		argv.push_back(nullptr);

		const pid_t pid = fork();
		if (pid < 0)
			return {ProcessOutcome::SpawnFailed, 0, 0};

		if (pid == 0)
		{
			execv(argv[0], argv.data());
			_exit(127);
		}

		const auto deadline = std::chrono::steady_clock::now() + timeout;

		for (;;)
		{
			int status = 0;
			if (waitpid(pid, &status, WNOHANG) == pid)
			{
				if (WIFSIGNALED(status))
					return {ProcessOutcome::Crashed, 0, static_cast<UInt32>(WTERMSIG(status))};

				return {ProcessOutcome::Exited, WEXITSTATUS(status), 0};
			}

			if (std::chrono::steady_clock::now() >= deadline)
			{
				kill(pid, SIGKILL);
				waitpid(pid, &status, 0);
				return {ProcessOutcome::TimedOut, 0, 0};
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
} // namespace cct

#endif
