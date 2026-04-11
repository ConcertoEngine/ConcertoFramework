//
// Created by arthur on 23/05/22.
//

#ifndef CONCERTO_GRAPHICS_WINDOW_HPP
#define CONCERTO_GRAPHICS_WINDOW_HPP

#include <string>
#include <functional>

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/Core/PixelFormat.hpp"
#include "Concerto/Graphics/Core/Window/Key.hpp"
#include "Concerto/Graphics/Core/Input/Input.hpp"
#include "Concerto/Graphics/Core/Window/NativeWindow.hpp"

struct SDL_Window;

namespace cct::gfx
{

	class CONCERTO_GRAPHICS_CORE_API Window
	{
	public:
		Window() = delete;
		Window(Int32 displayIndex, const std::string& title, Int32 width, Int32 height);
		~Window();

		Window(Window&&) noexcept = default;
		Window(const Window&) = delete;

		Window& operator=(Window&&) noexcept = default;
		Window& operator=(const Window&) = delete;

		NativeWindow GetNativeWindow() const;

		UInt32 GetWidth() const;
		UInt32 GetHeight() const;

		void SetTitle(const std::string& title);
		void SetCursorVisible(bool visible);
		void SetCursorIcon(const std::string& path);
		void SetCursorDisabled(bool disabled);

		bool ShouldClose() const;

		void RegisterResizeCallback(std::function<void(Window& window)> callback);
		void RegisterKeyCallback(std::function<void(Window& window, Key button, int scancode, int action, int mods)> callback);
		void RegisterMouseButtonCallback(std::function<void(Window& window, int button, int action, int mods)> callback);
		void RegisterCursorPosCallback(std::function<void(Window& window, double xpos, double ypos)> callback);

		Input& GetInputManager();

		UInt32 GetId() const;

		void SetShouldQuit(bool value);
		void TriggerResize();

		PixelFormat GetFormat() const;
	private:
		std::string m_title;
		std::size_t m_width;
		std::size_t m_height;
		SDL_Window* m_window;
		Input m_input;
		std::function<void(Window& window)> m_resizeCallback;
		std::function<void(Window& window, Key key, int scancode, int action, int mods)> m_keyCallback;
		std::function<void(Window& window, int button, int action, int mods)> m_mouseButtonCallback;
		std::function<void(Window& window, double xpos, double ypos)> m_cursorPosCallback;
		UInt32 m_windowID;
		bool m_shouldQuit;
	};
}

#endif //CONCERTO_GRAPHICS_WINDOW_HPP