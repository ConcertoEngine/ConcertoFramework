//
// Created by arthur on 23/05/22.
//

#include <stdexcept>

#include <Concerto/Core/Logger/Logger.hpp>
#include <Concerto/Core/Assert.hpp>

#include <SDL3/SDL.h>

#include "Concerto/Graphics/Core/Window/Window.hpp"

#include "Concerto/Profiler/Profiler.hpp"
#include "Concerto/Graphics/Core/Window/Event.hpp"

namespace cct::gfx
{
	namespace
	{
		MouseButton::Button SDLButtonToConcerto(UInt8 btn)
		{
			if (btn == SDL_BUTTON_LEFT)
				return MouseButton::Button::Left;
			if (btn == SDL_BUTTON_RIGHT)
				return MouseButton::Button::Right;
			if (btn == SDL_BUTTON_MIDDLE)
				return MouseButton::Button::Middle;
			if (btn == SDL_BUTTON_X1)
				return MouseButton::Button::Button1;
			if (btn == SDL_BUTTON_X2)
				return MouseButton::Button::Button2;
			CCT_ASSERT_FALSE("ConcertoGraphics: Invalid button value");
			return MouseButton::Button::Button5;
		}

		PixelFormat PixelFormatFromSDL(SDL_PixelFormat sdlFmt)
		{
			switch (sdlFmt)
			{
			case SDL_PIXELFORMAT_RGBA32:
			//case SDL_PIXELFORMAT_ABGR8888:
			case SDL_PIXELFORMAT_RGBA8888:
				return PixelFormat::RGBA8uNorm;

			case SDL_PIXELFORMAT_BGRA32:
			//case SDL_PIXELFORMAT_ARGB8888:
			case SDL_PIXELFORMAT_BGRA8888:
				return PixelFormat::BGRA8uNorm;

			// XRGB/RGBX/BGRX/XBGR = 8:8:8 + padding byte (alpha unused)
			// We map to the 4-channel equivalent, assuming alpha = 255
			case SDL_PIXELFORMAT_RGBX32:
			case SDL_PIXELFORMAT_XRGB32:
				return PixelFormat::RGBA8uNorm;

			case SDL_PIXELFORMAT_BGRX32:
			case SDL_PIXELFORMAT_XBGR32:
				return PixelFormat::BGRA8uNorm;

			// --- 24 bpp (3x8) ---
			case SDL_PIXELFORMAT_RGB24:
				return PixelFormat::RGB8uNorm;
			case SDL_PIXELFORMAT_BGR24:
				return PixelFormat::BGRuNorm;

			// --- Unsupported formats: palettized, packed, YUV, etc. ---
			case SDL_PIXELFORMAT_INDEX8:
			case SDL_PIXELFORMAT_INDEX4LSB:
			case SDL_PIXELFORMAT_INDEX4MSB:
			case SDL_PIXELFORMAT_INDEX2LSB:
			case SDL_PIXELFORMAT_INDEX2MSB:
			case SDL_PIXELFORMAT_INDEX1LSB:
			case SDL_PIXELFORMAT_INDEX1MSB:
			case SDL_PIXELFORMAT_XRGB4444:
			case SDL_PIXELFORMAT_XBGR4444:
			case SDL_PIXELFORMAT_ARGB4444:
			case SDL_PIXELFORMAT_RGBA4444:
			case SDL_PIXELFORMAT_ABGR4444:
			case SDL_PIXELFORMAT_BGRA4444:
			case SDL_PIXELFORMAT_XRGB1555:
			case SDL_PIXELFORMAT_XBGR1555:
			case SDL_PIXELFORMAT_ARGB1555:
			case SDL_PIXELFORMAT_RGBA5551:
			case SDL_PIXELFORMAT_ABGR1555:
			case SDL_PIXELFORMAT_BGRA5551:
			case SDL_PIXELFORMAT_RGB565:
			case SDL_PIXELFORMAT_BGR565:
			case SDL_PIXELFORMAT_ARGB2101010:
			case SDL_PIXELFORMAT_YV12:
			case SDL_PIXELFORMAT_IYUV:
			case SDL_PIXELFORMAT_YUY2:
			case SDL_PIXELFORMAT_UYVY:
			case SDL_PIXELFORMAT_YVYU:
			case SDL_PIXELFORMAT_NV12:
			case SDL_PIXELFORMAT_NV21:
			case SDL_PIXELFORMAT_EXTERNAL_OES:
			case SDL_PIXELFORMAT_UNKNOWN:
			default:
				throw std::runtime_error("Unsupported SDL pixel format: " + std::to_string(sdlFmt));
			}
		}

		Key SDLKeyToConcerto(SDL_Scancode scancode)
		{
			switch (scancode) {
			case SDL_SCANCODE_SPACE:
				return Key::Space;
			case SDL_SCANCODE_APOSTROPHE:
				return Key::Apostrophe;
			case SDL_SCANCODE_COMMA:
				return Key::Comma;
			case SDL_SCANCODE_MINUS:
				return Key::Minus;
			case SDL_SCANCODE_PERIOD:
				return Key::Period;
			case SDL_SCANCODE_SLASH:
				return Key::Slash;
			case SDL_SCANCODE_0:
				return Key::Zero;
			case SDL_SCANCODE_1:
				return Key::One;
			case SDL_SCANCODE_2:
				return Key::Two;
			case SDL_SCANCODE_3:
				return Key::Three;
			case SDL_SCANCODE_4:
				return Key::Four;
			case SDL_SCANCODE_5:
				return Key::Five;
			case SDL_SCANCODE_6:
				return Key::Six;
			case SDL_SCANCODE_7:
				return Key::Seven;
			case SDL_SCANCODE_8:
				return Key::Eight;
			case SDL_SCANCODE_9:
				return Key::Nine;
			case SDL_SCANCODE_SEMICOLON:
				return Key::Semicolon;
			case SDL_SCANCODE_EQUALS:
				return Key::Equal;
			case SDL_SCANCODE_A:
				return Key::A;
			case SDL_SCANCODE_B:
				return Key::B;
			case SDL_SCANCODE_C:
				return Key::C;
			case SDL_SCANCODE_D:
				return Key::D;
			case SDL_SCANCODE_E:
				return Key::E;
			case SDL_SCANCODE_F:
				return Key::F;
			case SDL_SCANCODE_G:
				return Key::G;
			case SDL_SCANCODE_H:
				return Key::H;
			case SDL_SCANCODE_I:
				return Key::I;
			case SDL_SCANCODE_J:
				return Key::J;
			case SDL_SCANCODE_K:
				return Key::K;
			case SDL_SCANCODE_L:
				return Key::L;
			case SDL_SCANCODE_M:
				return Key::M;
			case SDL_SCANCODE_N:
				return Key::N;
			case SDL_SCANCODE_O:
				return Key::O;
			case SDL_SCANCODE_P:
				return Key::P;
			case SDL_SCANCODE_Q:
				return Key::Q;
			case SDL_SCANCODE_R:
				return Key::R;
			case SDL_SCANCODE_S:
				return Key::S;
			case SDL_SCANCODE_T:
				return Key::T;
			case SDL_SCANCODE_U:
				return Key::U;
			case SDL_SCANCODE_V:
				return Key::V;
			case SDL_SCANCODE_W:
				return Key::W;
			case SDL_SCANCODE_X:
				return Key::X;
			case SDL_SCANCODE_Y:
				return Key::Y;
			case SDL_SCANCODE_Z:
				return Key::Z;
			case SDL_SCANCODE_LEFTBRACKET:
				return Key::LefBracket;
			case SDL_SCANCODE_BACKSLASH:
				return Key::BackSlash;
			case SDL_SCANCODE_RIGHTBRACKET:
				return Key::RightBracket;
			case SDL_SCANCODE_GRAVE:
				return Key::GraveAccent;
			case SDL_SCANCODE_ESCAPE:
				return Key::Escape;
			case SDL_SCANCODE_RETURN:
				return Key::Enter;
			case SDL_SCANCODE_TAB:
				return Key::TAB;
			case SDL_SCANCODE_BACKSPACE:
				return Key::Backspace;
			case SDL_SCANCODE_INSERT:
				return Key::Insert;
			case SDL_SCANCODE_DELETE:
				return Key::Delete;
			case SDL_SCANCODE_RIGHT:
				return Key::Right;
			case SDL_SCANCODE_LEFT:
				return Key::Left;
			case SDL_SCANCODE_DOWN:
				return Key::Down;
			case SDL_SCANCODE_UP:
				return Key::UP;
			case SDL_SCANCODE_PAGEUP:
				return Key::PageUp;
			case SDL_SCANCODE_PAGEDOWN:
				return Key::PageDown;
			case SDL_SCANCODE_HOME:
				return Key::Home;
			case SDL_SCANCODE_END:
				return Key::End;
			case SDL_SCANCODE_CAPSLOCK:
				return Key::CapsLock;
			case SDL_SCANCODE_SCROLLLOCK:
				return Key::ScrollLock;
			case SDL_SCANCODE_NUMLOCKCLEAR:
				return Key::NumLock;
			case SDL_SCANCODE_PRINTSCREEN:
				return Key::PrintScreen;
			case SDL_SCANCODE_PAUSE:
				return Key::Pause;
			case SDL_SCANCODE_F1:
				return Key::F1;
			case SDL_SCANCODE_F2:
				return Key::F2;
			case SDL_SCANCODE_F3:
				return Key::F3;
			case SDL_SCANCODE_F4:
				return Key::F4;
			case SDL_SCANCODE_F5:
				return Key::F5;
			case SDL_SCANCODE_F6:
				return Key::F6;
			case SDL_SCANCODE_F7:
				return Key::F7;
			case SDL_SCANCODE_F8:
				return Key::F8;
			case SDL_SCANCODE_F9:
				return Key::F9;
			case SDL_SCANCODE_F10:
				return Key::F10;
			case SDL_SCANCODE_F11:
				return Key::F11;
			case SDL_SCANCODE_F12:
				return Key::F12;
			case SDL_SCANCODE_F13:
				return Key::F13;
			case SDL_SCANCODE_F14:
				return Key::F14;
			case SDL_SCANCODE_F15:
				return Key::F15;
			case SDL_SCANCODE_F16:
				return Key::F16;
			case SDL_SCANCODE_F17:
				return Key::F17;
			case SDL_SCANCODE_F18:
				return Key::F18;
			case SDL_SCANCODE_F19:
				return Key::F19;
			case SDL_SCANCODE_F20:
				return Key::F20;
			case SDL_SCANCODE_F21:
				return Key::F21;
			case SDL_SCANCODE_F22:
				return Key::F22;
			case SDL_SCANCODE_F23:
				return Key::F23;
			case SDL_SCANCODE_F24:
				return Key::F24;
			case SDL_SCANCODE_KP_0:
				return Key::Kp0;
			case SDL_SCANCODE_KP_1:
				return Key::Kp1;
			case SDL_SCANCODE_KP_2:
				return Key::Kp2;
			case SDL_SCANCODE_KP_3:
				return Key::Kp3;
			case SDL_SCANCODE_KP_4:
				return Key::Kp4;
			case SDL_SCANCODE_KP_5:
				return Key::Kp5;
			case SDL_SCANCODE_KP_6:
				return Key::Kp6;
			case SDL_SCANCODE_KP_7:
				return Key::Kp7;
			case SDL_SCANCODE_KP_8:
				return Key::Kp8;
			case SDL_SCANCODE_KP_9:
				return Key::Kp9;
			case SDL_SCANCODE_KP_DECIMAL:
				return Key::KpDECIMAL;
			case SDL_SCANCODE_KP_DIVIDE:
				return Key::KpDIVIDE;
			case SDL_SCANCODE_KP_MULTIPLY:
				return Key::KpMULTIPLY;
			case SDL_SCANCODE_KP_MINUS:
				return Key::KpSUBTRACT;
			case SDL_SCANCODE_KP_PLUS:
				return Key::KpADD;
			case SDL_SCANCODE_KP_ENTER:
				return Key::KpENTER;
			case SDL_SCANCODE_KP_EQUALSAS400:
				return Key::KpEQUAL;
			case SDL_SCANCODE_KP_EQUALS:
				return Key::LeftShift;
			case SDL_SCANCODE_LSHIFT:
				return Key::LeftShift;
			case SDL_SCANCODE_LCTRL:
				return Key::LeftControl;
			case SDL_SCANCODE_LALT:
				return Key::LeftAlt;
			case SDL_SCANCODE_LGUI:
				return Key::RightShift;
			case SDL_SCANCODE_RSHIFT:
				return Key::RightShift;
			case SDL_SCANCODE_RCTRL:
				return Key::RightControl;
			case SDL_SCANCODE_RALT:
				return Key::RightAlt;
			case SDL_SCANCODE_RGUI:
				return Key::Menu;
			default:
				return Key::Undefined;
			}
		}

		bool EventHandler(void* userdata, SDL_Event* event)
		{
			Window* window = static_cast<Window*>(userdata);
			if (window == nullptr)
				return false;
			Event newEvent = {
				.type = Event::Type::Mouse,
				.name = "",
				.data = {}
			};
			switch (event->type)
			{
			case SDL_EVENT_WINDOW_RESIZED:
				if (event->window.windowID != window->GetId())
					return false;
				window->TriggerResize();
				return false;
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				if (event->window.windowID != window->GetId())
					return false;
				window->SetShouldQuit(true);
				return false;
			case SDL_EVENT_WINDOW_MAXIMIZED:
				if (event->window.windowID != window->GetId())
					return false;
				window->FireStateChange(WindowState::Maximized);
				return false;
			case SDL_EVENT_WINDOW_MINIMIZED:
				if (event->window.windowID != window->GetId())
					return false;
				window->FireStateChange(WindowState::Minimized);
				return false;
			case SDL_EVENT_WINDOW_RESTORED:
				if (event->window.windowID != window->GetId())
					return false;
				window->FireStateChange(WindowState::Normal);
				return false;
			case SDL_EVENT_MOUSE_MOTION:
			{
				if (event->motion.windowID != window->GetId())
					return false;
				MouseEvent mouseEvent = {};
				mouseEvent.type = MouseEvent::Type::Moved;
				mouseEvent.mouseMove.x = event->motion.x;
				mouseEvent.mouseMove.y = event->motion.y;
				mouseEvent.mouseMove.deltaX = event->motion.xrel;
				mouseEvent.mouseMove.deltaY = event->motion.yrel;
				newEvent.data = mouseEvent;
				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			{
				if (event->button.windowID != window->GetId())
					return false;
				MouseEvent mouseEvent = {};
				mouseEvent.type = MouseEvent::Type::Button;
				mouseEvent.button.button = SDLButtonToConcerto(event->button.button);
				mouseEvent.button.triggerType = TriggerType::Pressed;
				newEvent.data = mouseEvent;
				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				if (event->button.windowID != window->GetId())
					return false;
				MouseEvent mouseEvent = {};
				mouseEvent.type = MouseEvent::Type::Button;
				mouseEvent.button.button = SDLButtonToConcerto(event->button.button);
				mouseEvent.button.triggerType = TriggerType::Released;
				newEvent.data = mouseEvent;
				break;
			}
			case SDL_EVENT_KEY_DOWN:
			{
				if (event->key.windowID != window->GetId())
					return false;
				KeyEvent keyEvent = {};
				newEvent.type = Event::Type::Key;
				keyEvent.key = SDLKeyToConcerto(event->key.scancode);
				keyEvent.triggerType = TriggerType::Pressed;
				newEvent.data = keyEvent;
				break;
			}
			case SDL_EVENT_KEY_UP:
			{
				if (event->key.windowID != window->GetId())
					return false;
				KeyEvent keyEvent = {};
				newEvent.type = Event::Type::Key;
				keyEvent.key = SDLKeyToConcerto(event->key.scancode);
				keyEvent.triggerType = TriggerType::Released;
				newEvent.data = keyEvent;
				break;
			}
			case SDL_EVENT_MOUSE_WHEEL:
			{
				if (event->wheel.windowID != window->GetId())
					return false;
				MouseEvent wheelEvent = {};
				wheelEvent.type = MouseEvent::Type::Wheel;
				wheelEvent.mouseWheel.x = static_cast<int>(event->wheel.mouse_x);
				wheelEvent.mouseWheel.y = static_cast<int>(event->wheel.mouse_x);
				wheelEvent.mouseWheel.delta = event->wheel.y;
				newEvent.type = Event::Type::Mouse;
				newEvent.data = wheelEvent;
				break;
			}
			default:
				return false; //do not trigger event
			}
			window->GetInputManager().Trigger({ newEvent });
			return false;
		}
	}

	Window::Window(Int32 displayIndex, const std::string& title, Int32 width, Int32 height) :
		Window(displayIndex, title, width, height, false)
	{
	}

	Window::Window(Int32 displayIndex, const std::string& title, Int32 width, Int32 height, bool borderless) :
		m_title(title),
		m_width(width),
		m_height(height),
		m_window(nullptr),
		m_windowID(0),
		m_shouldQuit(false),
		m_titleBarHeight(0)
	{
		CCT_PROFILER_SCOPE();
		const auto displayId = static_cast<SDL_DisplayID>(displayIndex);
		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title.c_str());
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED_DISPLAY(displayId));
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED_DISPLAY(displayId));
		SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
		if (borderless)
			SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, true);
		m_window = SDL_CreateWindowWithProperties(props);
		SDL_DestroyProperties(props);
		if (m_window == nullptr)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: SDL_CreateWindowWithProperties failed: {}", SDL_GetError());
			throw std::runtime_error(std::format("ConcertoGraphics: SDL_CreateWindowWithProperties failed: {}", SDL_GetError()));
		}
		m_windowID = SDL_GetWindowID(m_window);

		if (m_windowID == 0)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Could not get window id message: {}", SDL_GetError());
			throw std::runtime_error(std::format("ConcertoGraphics: Could not get window id message: {}", SDL_GetError()));
		}

		SDL_AddEventWatch(EventHandler, this);
	}
	Window::~Window()
	{
		CCT_PROFILER_SCOPE();
		if (m_window != nullptr)
			SDL_SetWindowHitTest(m_window, nullptr, nullptr);
		SDL_RemoveEventWatch(EventHandler, this);
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

	void Window::SetTitle(const std::string& title)
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		SDL_SetWindowTitle(m_window, title.c_str());
	}


	void Window::SetCursorVisible(bool visible)
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		const bool ok = visible ? SDL_ShowCursor() : SDL_HideCursor();
		if (!ok)
			CCT_GFX_LOG_WARN("Window", "{}", SDL_GetError());
	}

	void Window::SetCursorIcon(const std::string& path)
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
	}

	void Window::SetCursorDisabled(bool disabled)
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		if (disabled)
		{
			if (!SDL_HideCursor())
				CCT_GFX_LOG_WARN("Window", "{}", SDL_GetError());
			if (!SDL_SetWindowRelativeMouseMode(m_window, true))
				CCT_GFX_LOG_WARN("Window", "{}", SDL_GetError());
		}
		else
		{
			if (!SDL_ShowCursor())
				CCT_GFX_LOG_WARN("Window", "{}", SDL_GetError());
			if (!SDL_SetWindowRelativeMouseMode(m_window, false))
				CCT_GFX_LOG_WARN("Window", "{}", SDL_GetError());
		}
	}

	UInt32 Window::GetHeight() const
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		int w, h;
		SDL_GetWindowSize(m_window, &w, &h);
		return static_cast<UInt32>(h);
	}

	UInt32 Window::GetWidth() const
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		int w, h;
		SDL_GetWindowSize(m_window, &w, &h);
		return static_cast<UInt32>(w);
	}

	NativeWindow Window::GetNativeWindow() const
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		const SDL_PropertiesID props = SDL_GetWindowProperties(m_window);
		if (props == 0)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Could not get window properties message: {}", SDL_GetError());
			return {};
		}
		NativeWindow nativeWindow;
#if defined(CCT_PLATFORM_WINDOWS)
		nativeWindow.window = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
		nativeWindow.hinstance = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
#elif defined(CCT_PLATFORM_MACOS)
		CCT_ASSERT_FALSE("Not implemented");
#elif defined(CCT_PLATFORM_LINUX)
		if (void* wlDisplay = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr))
		{
			void* wlSurface = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
			nativeWindow.platform = NativeWindow::Wayland{ wlDisplay, wlSurface };
		}
		else if (void* x11Display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr))
		{
			const auto x11Window = static_cast<unsigned long>(SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
			nativeWindow.platform = NativeWindow::X11{ x11Display, x11Window };
		}
		else
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Unsupported Linux windowing system");
			throw std::runtime_error("ConcertoGraphics: Unsupported Linux windowing system");
		}
#endif
		return nativeWindow;
	}

	bool Window::ShouldClose() const
	{
		return m_shouldQuit;
	}

	void Window::RegisterResizeCallback(std::function<void(Window& window)> callback)
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		m_resizeCallback = std::move(callback);
	}

	void Window::RegisterKeyCallback(std::function<void(Window&, Key, int, int, int)> callback)
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		m_keyCallback = std::move(callback);
	}

	void Window::RegisterMouseButtonCallback(std::function<void(Window& window, int button, int action, int mods)> callback)
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		m_mouseButtonCallback = std::move(callback);
	}

	void Window::RegisterCursorPosCallback(std::function<void(Window& window, double xpos, double ypos)> callback)
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		m_cursorPosCallback = std::move(callback);
	}

	Input& Window::GetInputManager()
	{
		return m_input;
	}

	UInt32 Window::GetId() const
	{
		return m_windowID;
	}

	void Window::SetShouldQuit(bool value)
	{
		m_shouldQuit = value;
	}

	void Window::TriggerResize()
	{
		if (m_resizeCallback)
			m_resizeCallback(*this);
	}

	PixelFormat Window::GetFormat() const
	{
		return PixelFormatFromSDL(SDL_GetWindowPixelFormat(m_window));
	}

	void Window::RegisterStateChangeCallback(std::function<void(Window&, WindowState)> callback)
	{
		m_stateCallback = std::move(callback);
	}

	void Window::Minimize()
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		SDL_MinimizeWindow(m_window);
	}

	void Window::Maximize()
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		SDL_MaximizeWindow(m_window);
	}

	void Window::Restore()
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		SDL_RestoreWindow(m_window);
	}

	void Window::ToggleMaximize()
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		const SDL_WindowFlags flags = SDL_GetWindowFlags(m_window);
		const bool isMaximized = (flags & SDL_WINDOW_MAXIMIZED) != 0;
		const bool isResizable = (flags & SDL_WINDOW_RESIZABLE) != 0;
		if (isMaximized)
		{
			SDL_RestoreWindow(m_window);
		}
		else
		{
			// SDL_MaximizeWindow is a no-op on non-resizable windows. On Windows the
			// SDL_WINDOW_BORDERLESS flag silently strips RESIZABLE — re-assert it.
			if (!isResizable)
				SDL_SetWindowResizable(m_window, true);
			SDL_MaximizeWindow(m_window);
		}
	}

	void Window::Close()
	{
		m_shouldQuit = true;
	}

	WindowState Window::GetState() const
	{
		if (m_window == nullptr)
			return WindowState::Normal;
		const SDL_WindowFlags flags = SDL_GetWindowFlags(m_window);
		if ((flags & SDL_WINDOW_MAXIMIZED) != 0)
			return WindowState::Maximized;
		if ((flags & SDL_WINDOW_MINIMIZED) != 0)
			return WindowState::Minimized;
		return WindowState::Normal;
	}

	void Window::FireStateChange(WindowState state)
	{
		if (m_stateCallback)
			m_stateCallback(*this, state);
	}

	namespace
	{
		SDL_HitTestResult SDLCALL HitTestThunk(SDL_Window* /*win*/, const SDL_Point* p, void* data)
		{
			auto* self = static_cast<Window*>(data);
			return static_cast<SDL_HitTestResult>(self->HitTest(p->x, p->y));
		}
	}

	void Window::SetDraggableRegions(int titleBarHeight, std::vector<DraggableRect> nonDraggableRects)
	{
		CCT_ASSERT(m_window, "ConcertoGraphics: invalid window pointer");
		m_titleBarHeight = titleBarHeight;
		m_nonDraggableRects = std::move(nonDraggableRects);
		// SDL_SetWindowHitTest is idempotent — calling it again replaces the callback.
		if (!SDL_SetWindowHitTest(m_window, &HitTestThunk, this))
			CCT_GFX_LOG_WARN("Window", "SDL_SetWindowHitTest failed: {}", SDL_GetError());
	}

	void Window::ClearDraggableRegions()
	{
		m_titleBarHeight = 0;
		m_nonDraggableRects.clear();
		if (m_window != nullptr)
			SDL_SetWindowHitTest(m_window, nullptr, nullptr);
	}

	HitTestResult Window::HitTest(int x, int y) const
	{
		const int W = static_cast<int>(GetWidth());
		const int H = static_cast<int>(GetHeight());
		constexpr int kBorder = 6; // resize handle thickness

		const bool L = x < kBorder;
		const bool R = x >= W - kBorder;
		const bool T = y < kBorder;
		const bool B = y >= H - kBorder;

		if (T && L) return HitTestResult::ResizeTopLeft;
		if (T && R) return HitTestResult::ResizeTopRight;
		if (B && L) return HitTestResult::ResizeBottomLeft;
		if (B && R) return HitTestResult::ResizeBottomRight;
		if (T)      return HitTestResult::ResizeTop;
		if (B)      return HitTestResult::ResizeBottom;
		if (L)      return HitTestResult::ResizeLeft;
		if (R)      return HitTestResult::ResizeRight;

		if (y < m_titleBarHeight)
		{
			for (const auto& r : m_nonDraggableRects)
			{
				if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h)
					return HitTestResult::Normal;
			}
			return HitTestResult::Draggable;
		}
		return HitTestResult::Normal;
	}
}
