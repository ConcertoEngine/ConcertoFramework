//
// Created by arthur on 02/06/2024.
//


#include "Concerto/Core/Error/Error.hpp"

#ifdef CCT_PLATFORM_WINDOWS
#include <windows.h>
#elif defined(CCT_PLATFORM_POSIX)
#include <cstring>
#include <errno.h>
#else
#error Unsupported platform
#endif

namespace cct
{
	std::string Error::GetLastSystemErrorString()
	{
#ifdef CCT_PLATFORM_WINDOWS
		const DWORD error = GetLastError();
#ifdef CCT_DEBUG // This may help for debug
		const auto hResult = HRESULT_FROM_WIN32(error);
		[[maybe_unused]] const auto facility = HRESULT_FACILITY(hResult);
		[[maybe_unused]] const auto severity = HRESULT_SEVERITY(hResult);
		[[maybe_unused]] const auto code = HRESULT_CODE(hResult);
#endif
		LPSTR messageBuffer = nullptr;

		const size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
										   nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr);

		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);

		return message;
#elif defined(CCT_PLATFORM_POSIX)
		return std::strerror(errno);
#else
#error Unsupported platform
		return "Not supported";
#endif
	}
}

