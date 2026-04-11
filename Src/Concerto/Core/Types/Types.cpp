//
// Created by arthur on 05/08/2023.
//

#include "Concerto/Core/Types/Types.hpp"

#if defined(CCT_PLATFORM_WINDOWS)
#include <windows.h>
namespace cct
{
	bool IsDebuggerAttached()
	{
		return IsDebuggerPresent() != 0;
	}
}
#elif defined(CCT_PLATFORM_POSIX)
#include <fstream>
namespace cct
{
	bool IsDebuggerAttached()
	{
		std::ifstream status_file("/proc/self/status");
		std::string line;
		while (std::getline(status_file, line))
		{
			if (line.substr(0, 11) == "TracerPid:\t")
			{
				int tracerPid = std::stoi(line.substr(11));
				return tracerPid != 0;
			}
		}
		return false;
	}
}
#else
namespace cct
{
	bool IsDebuggerAttached()
	{
		return false;
	}
}
#endif