//
// Created by arthur on 201/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_DX12_DEFINES_HPP
#define CONCERTO_GRAPHICS_BACKEND_DX12_DEFINES_HPP

#include <Concerto/Core/Types/Types.hpp>

#ifdef CCT_COMPILER_MSVC
#pragma warning(disable : 4251) // Disable warning about DLL interface needed
#endif

#ifdef CONCERTO_GRAPHICS_DX12_BACKEND_BUILD
#define CONCERTO_GRAPHICS_DX12_BACKEND_API CCT_EXPORT
#else
#define CONCERTO_GRAPHICS_DX12_BACKEND_API CCT_IMPORT
#endif // CONCERTO_GRAPHICS_DX12_BACKEND_BUILD

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <Windows.h>
#include <wrl.h>
#undef min
#undef max
#undef OutputDebugString

#include <vector>

#include <Concerto/Core/Assert.hpp>
#include <Concerto/Graphics/Core/Defines.hpp>

#include "Concerto/Graphics/Backend/Dx12/Dx12Exception.hpp"

#define CCT_DX12_LOG_TRACE(channel, fmt, ...) CCT_LOG_TRACE("Graphics", "DX12-Backend", fmt __VA_OPT__(, ) __VA_ARGS__))
#define CCT_DX12_LOG_DEBUG(channel, fmt, ...) CCT_LOG_DEBUG("Graphics", "DX12-Backend", fmt __VA_OPT__(, ) __VA_ARGS__))
#define CCT_DX12_LOG_INFO(channel, fmt, ...) CCT_LOG_INFO("Graphics", "DX12-Backend", fmt __VA_OPT__(, ) __VA_ARGS__))
#define CCT_DX12_LOG_WARN(channel, fmt, ...) CCT_LOG_WARN("Graphics", "DX12-Backend", fmt __VA_OPT__(, ) __VA_ARGS__))
#define CCT_DX12_LOG_ERROR(channel, fmt, ...) CCT_LOG_ERROR("Graphics", "DX12-Backend", fmt __VA_OPT__(, ) __VA_ARGS__))
#define CCT_DX12_LOG_CRITICAL(channel, fmt, ...) CCT_LOG_CRITICAL("Graphics", "DX12-Backend", fmt __VA_OPT__(, ) __VA_ARGS__))

#endif // CONCERTO_GRAPHICS_BACKEND_DX12_DEFINES_HPP