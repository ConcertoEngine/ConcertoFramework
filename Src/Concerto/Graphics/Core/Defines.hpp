//
// Created by arthur on 24/08/2023.
//

#ifndef CONCERTO_GRAPHICS_DEFINES_HPP
#define CONCERTO_GRAPHICS_DEFINES_HPP

#include <cstring>
#include <Concerto/Core/Types/Types.hpp>
#include <Concerto/Core/Logger/LogMacros.hpp>

#define CCT_GFX_LOG_TRACE(channel, fmt, ...) CCT_LOG_TRACE("Graphics", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_GFX_LOG_DEBUG(channel, fmt, ...) CCT_LOG_DEBUG("Graphics", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_GFX_LOG_INFO(channel, fmt, ...) CCT_LOG_INFO("Graphics", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_GFX_LOG_WARN(channel, fmt, ...) CCT_LOG_WARN("Graphics", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_GFX_LOG_ERROR(channel, fmt, ...) CCT_LOG_ERROR("Graphics", channel, fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_GFX_LOG_CRITICAL(channel, fmt, ...) CCT_LOG_CRITICAL("Graphics", channel, fmt __VA_OPT__(, ) __VA_ARGS__)


#ifdef CCT_COMPILER_MSVC
#pragma warning(disable: 4251) // Disable warning about DLL interface needed
#endif

#ifdef CONCERTO_GRAPHICS_CORE_BUILD
	#define CONCERTO_GRAPHICS_CORE_API CCT_EXPORT
#else
	#define CONCERTO_GRAPHICS_CORE_API CCT_IMPORT
#endif // CONCERTO_GRAPHICS_CORE_BUILD

#endif //CONCERTO_GRAPHICS_DEFINES_HPP