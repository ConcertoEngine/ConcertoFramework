//
// Created by arthur on 24/08/2023.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_VULKAN_DEFINES_HPP
#define CONCERTO_GRAPHICS_BACKEND_VULKAN_DEFINES_HPP

#include <Concerto/Core/Types/Types.hpp>

#ifdef CCT_COMPILER_MSVC
#pragma warning(disable : 4251) // Disable warning about DLL interface needed
#endif

#ifdef CONCERTO_GRAPHICS_VULKAN_BACKEND_BUILD
#define CONCERTO_GRAPHICS_VULKAN_BACKEND_API CCT_EXPORT
#else
#define CONCERTO_GRAPHICS_VULKAN_BACKEND_API CCT_IMPORT
#endif // CONCERTO_GRAPHICS_VULKAN_BACKEND_BUILD
#define VOLK_VULKAN_H_PATH <string>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#ifdef CCT_PLATFORM_WINDOWS
typedef unsigned long DWORD;
typedef const wchar_t* LPCWSTR;
typedef void* HANDLE;
typedef struct HINSTANCE__* HINSTANCE;
typedef struct HWND__* HWND;
typedef struct HMONITOR__* HMONITOR;
typedef struct m_SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan_win32.h>
#elif defined(CCT_PLATFORM_LINUX)
#if defined(CCT_GFX_XLIB)
typedef struct _XDisplay Display;
typedef unsigned long XID;
typedef XID Window;
typedef unsigned long VisualID;
#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan_xlib.h>
#endif
#if defined(CCT_GFX_WAYLAND)
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan_wayland.h>
#endif
#undef None
#endif

#ifndef VULKAN_H_
#define VULKAN_H_ // define VULKAN_H_ to avoid third party libraries to include <vulkan/vulkan.h>
#endif

#include <Concerto/Core/Result/Result.hpp>
#include <Concerto/Graphics/Core/Defines.hpp>
#include <Concerto/Profiler/Profiler.hpp>

#define CCT_VK_LOG_TRACE(fmt, ...) CCT_GFX_LOG_TRACE("Vulkan-Backend", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_VK_LOG_DEBUG(fmt, ...) CCT_GFX_LOG_DEBUG("Vulkan-Backend", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_VK_LOG_INFO(fmt, ...) CCT_GFX_LOG_INFO("Vulkan-Backend", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_VK_LOG_WARN(fmt, ...) CCT_GFX_LOG_WARN("Vulkan-Backend", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_VK_LOG_ERROR(fmt, ...) CCT_GFX_LOG_ERROR("Vulkan-Backend", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_VK_LOG_CRITICAL(fmt, ...) CCT_GFX_LOG_CRITICAL("Vulkan-Backend", fmt __VA_OPT__(, ) __VA_ARGS__)
#endif // CONCERTO_GRAPHICS_BACKEND_VULKAN_DEFINES_HPP