//
// Created by arthur on 28/10/2024.
//

#ifndef CONCERTO_GRAPHICS_NATIVEWINDOW_HPP
#define CONCERTO_GRAPHICS_NATIVEWINDOW_HPP

#include "Concerto/Graphics/Core/Defines.hpp"

#if defined(CCT_PLATFORM_LINUX)
#include <variant>
#endif // CCT_PLATFORM_LINUX

namespace cct::gfx
{

#if defined(CCT_PLATFORM_WINDOWS)
	struct NativeWindow
	{
		void* window;
		void* hinstance;
	};
#elif defined(CCT_PLATFORM_MACOS)
	struct NativeWindow
	{

	};
#elif defined(CCT_PLATFORM_LINUX)
	struct NativeWindow
	{
		struct X11
		{
			void* display;
			unsigned long window;
		};

		struct Wayland
		{
			void* wl_display;
			void* wl_surface;
		};

		std::variant<X11, Wayland> platform;
	};
#endif
}

#endif //CONCERTO_GRAPHICS_NATIVEWINDOW_HPP