//
// Created by arthur on 27/10/2024.
//

#include <SDL3/SDL.h>
#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Core/DisplayManager/DisplayManager.hpp"

#include "Concerto/Profiler/Profiler.hpp"

namespace cct::gfx
{
	namespace
	{
		PixelFormat PixelFormatFrom(SDL_PixelFormat pixelFormat)
		{
			switch (pixelFormat) {
			case SDL_PIXELFORMAT_XRGB8888:
				return PixelFormat::RGB8uNorm;
			case SDL_PIXELFORMAT_RGB24:
				return PixelFormat::RGB8uNorm;
			case SDL_PIXELFORMAT_RGBA8888:
				return PixelFormat::RGBA8uNorm;
			case SDL_PIXELFORMAT_ARGB8888:
				return PixelFormat::BGRA8uNorm; // Assuming BGRA is ARGB
			case SDL_PIXELFORMAT_XBGR8888:
				return PixelFormat::BGRuNorm;
			case SDL_PIXELFORMAT_ABGR8888:
				return PixelFormat::BGRA8uNorm;
			case SDL_PIXELFORMAT_RGB565:
				return PixelFormat::RGB8uNorm; // Assuming it's closest to 8-bit normalized
			case SDL_PIXELFORMAT_RGBA5551:
				return PixelFormat::RGBA8uNorm;
			case SDL_PIXELFORMAT_RGB332:
				return PixelFormat::RGB8uNorm; // Approximated to RGB8
			case SDL_PIXELFORMAT_XRGB4444:
				return PixelFormat::RGB8uNorm;
			case SDL_PIXELFORMAT_XRGB1555:
				return PixelFormat::RGB8uNorm;
			default:
				throw std::invalid_argument("Unsupported SDL format");
			}
		}
	}
	DisplayManager::DisplayManager()
	{
		CCT_PROFILER_SCOPE();
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: SDL initialization failed message: {}", SDL_GetError());
			throw std::runtime_error(std::format("SDL initialization failed message: {}", SDL_GetError()));
		}
	}

	DisplayManager::~DisplayManager()
	{
		SDL_Quit();
	}

	std::vector<DisplayInfo> DisplayManager::EnumerateDisplaysInfos()
	{
		CCT_PROFILER_SCOPE();
		std::vector<DisplayInfo> displayInfos;
		int numDisplay = 0;
		SDL_DisplayID* displays = SDL_GetDisplays(&numDisplay);
		if (displays == nullptr)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Display enumeration failed message: {}", SDL_GetError());
			return {};
		}
		const SDL_DisplayID primary = SDL_GetPrimaryDisplay();

		for (int i = 0; i < numDisplay; ++i)
		{
			CCT_PROFILER_SCOPE("Enumerate display");
			const SDL_DisplayID displayId = displays[i];
			const char* displayName = SDL_GetDisplayName(displayId);
			if (displayName == nullptr)
			{
				CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't get display name message: {}", SDL_GetError());
				continue;
			}
			SDL_Rect displayBounds;
			if (!SDL_GetDisplayBounds(displayId, &displayBounds))
			{
				CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't get display bounds message: {}", SDL_GetError());
				continue;
			}
			static_assert(sizeof(SDL_Rect) == sizeof(DisplayInfo::Bounds) && "Invalid Bounds size");

			int numDisplayMode = 0;
			SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displayId, &numDisplayMode);
			std::vector<DisplayInfo::DisplayMode> displayModes;
			if (modes != nullptr)
			{
				for (int displayModeIndex = 0; displayModeIndex < numDisplayMode; ++displayModeIndex)
				{
					const SDL_DisplayMode* sdlDisplayMode = modes[displayModeIndex];
					DisplayInfo::DisplayMode displayMode = {
						.displayModeIndex = displayModeIndex,
						.pixelFormat = PixelFormatFrom(sdlDisplayMode->format),
						.width = sdlDisplayMode->w,
						.height = sdlDisplayMode->h,
						.refreshRate = static_cast<Int32>(sdlDisplayMode->refresh_rate)
					};
					displayModes.emplace_back(std::move(displayMode));
				}
				SDL_free(modes);
			}
			else
			{
				CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't get display modes message: {}", SDL_GetError());
			}

			DisplayInfo displayInfo = {
				.displayIndex = static_cast<Int32>(displayId),
				.displayName = std::string_view(displayName, std::strlen(displayName)),
				.displayBounds = {}, //filled below with std::memcpy
				.isPrimary = (displayId == primary),
				.displayModes = std::move(displayModes)
			};
			std::memcpy(&displayInfo.displayBounds, &displayBounds, sizeof(DisplayInfo::Bounds));
			displayInfos.emplace_back(std::move(displayInfo));
		}
		SDL_free(displays);
		return displayInfos;
	}

	std::unique_ptr<Window> DisplayManager::CreateWindow(Int32 displayIndex, const std::string& name, Int32 width, Int32 height)
	{
		return std::make_unique<Window>(displayIndex, name, width, height);
	}

	std::unique_ptr<Window> DisplayManager::CreateWindow(Int32 displayIndex, const std::string& name, Int32 width, Int32 height, bool borderless)
	{
		return std::make_unique<Window>(displayIndex, name, width, height, borderless);
	}

	void DisplayManager::DispatchEvents()
	{
		SDL_PumpEvents();
	}
}
