//
// Created by arthur on 14/08/2026.
//

#include "Concerto/Core/Paths/Paths.hpp"

#ifdef CCT_PLATFORM_WINDOWS
#include <shlobj.h>
#include <windows.h>
#elif defined(CCT_PLATFORM_POSIX)
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>
#else
#error Unsupported platform
#endif

namespace
{
#ifdef CCT_PLATFORM_WINDOWS
	std::filesystem::path FromKnownFolder(REFKNOWNFOLDERID id)
	{
		PWSTR raw = nullptr;
		if (FAILED(SHGetKnownFolderPath(id, 0, nullptr, &raw)) || raw == nullptr)
			return {};
		const std::filesystem::path result(raw);
		CoTaskMemFree(raw);
		return result;
	}
#elif defined(CCT_PLATFORM_POSIX)
	std::filesystem::path HomeDir()
	{
		if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0')
			return std::filesystem::path(home);
		if (const passwd* pw = getpwuid(getuid()); pw != nullptr && pw->pw_dir != nullptr)
			return std::filesystem::path(pw->pw_dir);
		return {};
	}

	std::filesystem::path HomeRelative(std::string_view subdir)
	{
		const std::filesystem::path home = HomeDir();
		return home.empty() ? std::filesystem::path{} : home / subdir;
	}
#endif
} // namespace

namespace cct
{
	std::filesystem::path Paths::Get(StandardDirectory dir)
	{
#ifdef CCT_PLATFORM_WINDOWS
		switch (dir)
		{
			case StandardDirectory::Temp:
			{
				wchar_t buffer[MAX_PATH + 1];
				const DWORD len = GetTempPathW(MAX_PATH, buffer);
				if (len == 0 || len > MAX_PATH)
					return {};
				return std::filesystem::path(buffer, buffer + len);
			}
			case StandardDirectory::Home:
				return FromKnownFolder(FOLDERID_Profile);
			case StandardDirectory::Documents:
				return FromKnownFolder(FOLDERID_Documents);
			case StandardDirectory::Downloads:
				return FromKnownFolder(FOLDERID_Downloads);
			case StandardDirectory::Videos:
				return FromKnownFolder(FOLDERID_Videos);
			case StandardDirectory::Pictures:
				return FromKnownFolder(FOLDERID_Pictures);
			case StandardDirectory::Music:
				return FromKnownFolder(FOLDERID_Music);
			case StandardDirectory::Desktop:
				return FromKnownFolder(FOLDERID_Desktop);
		}
		return {};
#elif defined(CCT_PLATFORM_POSIX)
		switch (dir)
		{
			case StandardDirectory::Temp:
			{
				if (const char* tmp = std::getenv("TMPDIR"); tmp != nullptr && tmp[0] != '\0')
					return std::filesystem::path(tmp);
				return "/tmp";
			}
			case StandardDirectory::Home:
				return HomeDir();
			case StandardDirectory::Documents:
				return HomeRelative("Documents");
			case StandardDirectory::Downloads:
				return HomeRelative("Downloads");
			case StandardDirectory::Videos:
#ifdef CCT_PLATFORM_MACOS
				return HomeRelative("Movies");
#else
				return HomeRelative("Videos");
#endif
			case StandardDirectory::Pictures:
				return HomeRelative("Pictures");
			case StandardDirectory::Music:
				return HomeRelative("Music");
			case StandardDirectory::Desktop:
				return HomeRelative("Desktop");
		}
		return {};
#else
#error Unsupported platform
#endif
	}
} // namespace cct
