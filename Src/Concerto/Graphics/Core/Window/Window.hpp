//
// Created by arthur on 23/05/22.
//

#ifndef CONCERTO_GRAPHICS_WINDOW_HPP
#define CONCERTO_GRAPHICS_WINDOW_HPP

#include <functional>
#include <string>
#include <vector>

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/Core/Input/Input.hpp"
#include "Concerto/Graphics/Core/PixelFormat.hpp"
#include "Concerto/Graphics/Core/Window/Key.hpp"
#include "Concerto/Graphics/Core/Window/NativeWindow.hpp"

struct SDL_Window;

namespace cct::gfx
{
	enum class WindowState
	{
		Normal,
		Minimized,
		Maximized
	};

	enum class HitTestResult : int
	{
		Normal = 0,
		Draggable = 1,
		ResizeTopLeft = 2,
		ResizeTop = 3,
		ResizeTopRight = 4,
		ResizeRight = 5,
		ResizeBottomRight = 6,
		ResizeBottom = 7,
		ResizeBottomLeft = 8,
		ResizeLeft = 9,
	};

	class CONCERTO_GRAPHICS_CORE_API Window
	{
	public:
		struct DraggableRect
		{
			int x;
			int y;
			int w;
			int h;
		};

		Window() = delete;
		Window(Int32 displayIndex, const std::string& title, Int32 width, Int32 height);
		Window(Int32 displayIndex, const std::string& title, Int32 width, Int32 height, bool borderless);
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
		void RegisterStateChangeCallback(std::function<void(Window& window, WindowState state)> callback);

		Input& GetInputManager();

		UInt32 GetId() const;

		void SetShouldQuit(bool value);
		void TriggerResize();

		// Window chrome operations.
		void Minimize();
		void Maximize();
		void Restore();
		void ToggleMaximize();
		void Close();
		WindowState GetState() const;

		void SetDraggableRegions(int titleBarHeight, std::vector<DraggableRect> nonDraggableRects);
		void ClearDraggableRegions();

		HitTestResult HitTest(int x, int y) const;

		PixelFormat GetFormat() const;

		void FireStateChange(WindowState state);
		void FireTextInput(const char* text);

		void SetTextInputCallback(std::function<void(const char*)> cb);
		void StartTextInput();
		void StopTextInput();

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
		std::function<void(Window& window, WindowState state)> m_stateCallback;
		std::function<void(const char*)> m_textInputCb;
		UInt32 m_windowID;
		bool m_shouldQuit;
		int m_titleBarHeight;
		std::vector<DraggableRect> m_nonDraggableRects;
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_WINDOW_HPP