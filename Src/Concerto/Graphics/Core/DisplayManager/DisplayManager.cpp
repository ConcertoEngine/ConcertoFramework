//
// Created by arthur on 27/10/2024.
//

#include <SDL2/SDL.h>
#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Core/DisplayManager/DisplayManager.hpp"

#include "Concerto/Profiler/Profiler.hpp"

namespace cct::gfx
{
	namespace
	{
		PixelFormat PixelFormatFrom(UInt32 pixelFormat)
		{
			switch (pixelFormat) {
			case SDL_PIXELFORMAT_RGB888:
				return PixelFormat::RGB8uNorm;
			case SDL_PIXELFORMAT_RGB24:
				return PixelFormat::RGB8uNorm;
			case SDL_PIXELFORMAT_RGBA8888:
				return PixelFormat::RGBA8uNorm;
			case SDL_PIXELFORMAT_ARGB8888:
				return PixelFormat::BGRA8uNorm; // Assuming BGRA is ARGB
			case SDL_PIXELFORMAT_BGR888:
				return PixelFormat::BGRuNorm;
			case SDL_PIXELFORMAT_ABGR8888:
				return PixelFormat::BGRA8uNorm;
			case SDL_PIXELFORMAT_RGB565:
				return PixelFormat::RGB8uNorm; // Assuming it’s closest to 8-bit normalized
			case SDL_PIXELFORMAT_RGBA5551:
				return PixelFormat::RGBA8uNorm;
			case SDL_PIXELFORMAT_RGB332:
				return PixelFormat::RGB8uNorm; // Approximated to RGB8
			case SDL_PIXELFORMAT_RGB444:
				return PixelFormat::RGB8uNorm;
			case SDL_PIXELFORMAT_RGB555:
				return PixelFormat::RGB8uNorm;
			default:
				throw std::invalid_argument("Unsupported SDL format");
			}
		}
	}
	DisplayManager::DisplayManager()
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();
		int result = SDL_Init(SDL_INIT_VIDEO);
		if (result < 0)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: SDL initialization failed error code: {}, message {}", result, SDL_GetError());
			throw std::runtime_error(std::format("SDL initialization failed error code: {}, message {}", result, SDL_GetError()));
		}
	}

	DisplayManager::~DisplayManager()
	{
		SDL_Quit();
	}

	std::vector<DisplayInfo> DisplayManager::EnumerateDisplaysInfos()
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();
		std::vector<DisplayInfo> displayInfos;
		const Int32 numDisplay = SDL_GetNumVideoDisplays();
		if (numDisplay < 0)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Display enumeration failed code: {}, message: {}", numDisplay, SDL_GetError());
			return {};
		}

		for (Int32 displayIndex = 0; displayIndex < numDisplay; ++displayIndex)
		{
			CCT_GFX_PROFILER_SCOPE("Enumerate display");
			const char* displayName = SDL_GetDisplayName(displayIndex);
			if (displayName == nullptr)
			{
				CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't get display name message: {}", SDL_GetError());
				continue;
			}
			SDL_Rect displayBounds;
			Int32 result = SDL_GetDisplayBounds(displayIndex, &displayBounds);
			if (result < 0)
			{
				CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't get display bounds code: {}, message: {}", result, SDL_GetError());
				continue;
			}
			static_assert(sizeof(SDL_Rect) == sizeof(DisplayInfo::Bounds) && "Invalid Bounds size");


			Int32 numDisplayMode = SDL_GetNumDisplayModes(displayIndex);
			if (numDisplayMode < 0)
			{
				CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't get display bounds code: {}, message: {}", result, SDL_GetError());
				continue;
			}

			std::vector<DisplayInfo::DisplayMode> displayModes;
			for (Int32 displayModeIndex = 0; displayModeIndex < numDisplayMode; ++displayModeIndex)
			{
				SDL_DisplayMode sdlDisplayMode = {};
				result = SDL_GetDisplayMode(displayIndex, displayModeIndex, &sdlDisplayMode);
				if (result < 0)
				{
					CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't get display mode code: {}, message: {}", result, SDL_GetError());
					continue;
				}
				DisplayInfo::DisplayMode displayMode = {
					.displayModeIndex = displayModeIndex,
					.pixelFormat = PixelFormatFrom(sdlDisplayMode.format),
					.width = sdlDisplayMode.w,
					.height = sdlDisplayMode.h,
					.refreshRate = sdlDisplayMode.refresh_rate
				};
				displayModes.emplace_back(std::move(displayMode));
			}

			DisplayInfo displayInfo = {
				.displayIndex = displayIndex,
				.displayName = std::string_view(displayName, std::strlen(displayName)),
				.displayBounds = {}, //filled below with std::memcpy
				.isPrimary = displayIndex == 0,
				.displayModes = std::move(displayModes)
			};
			std::memcpy(&displayInfo.displayBounds, &displayBounds, sizeof(DisplayInfo::Bounds));
			displayInfos.emplace_back(std::move(displayInfo));
		}
		return displayInfos;
	}

	std::unique_ptr<Window> DisplayManager::CreateWindow(Int32 displayIndex, const std::string& name, Int32 width, Int32 height)
	{
		return std::make_unique<Window>(displayIndex, name, width, height);
	}

	void DisplayManager::DispatchEvents()
	{
		SDL_PumpEvents();
	}
}
