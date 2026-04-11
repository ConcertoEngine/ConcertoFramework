//
// Created by arthur on 18/08/2025.
//

#include "Concerto/Core/Logger/Logger.hpp"

#ifdef CCT_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace cct
{
	void Logger::DebugString(std::string_view string)
	{
#ifdef CCT_PLATFORM_WINDOWS
		OutputDebugString(string.data());
#else
		std::cerr << string;
#endif
	}
}
