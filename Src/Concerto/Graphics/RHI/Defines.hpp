//
// Created by arthur on 24/08/2023.
//

#ifndef CONCERTO_GRAPHICS_RHI_DEFINES_HPP
#define CONCERTO_GRAPHICS_RHI_DEFINES_HPP

#include <Concerto/Core/Types/Types.hpp>
#include <Concerto/Core/Assert.hpp>

#ifdef CCT_COMPILER_MSVC
#pragma warning(disable: 4251) // Disable warning about DLL interface needed
#endif

#ifdef CONCERTO_GRAPHICS_RHI_MODULE_BUILD
#define CONCERTO_GRAPHICS_RHI_BASE_API CCT_EXPORT
#else
#define CONCERTO_GRAPHICS_RHI_BASE_API CCT_IMPORT
#endif // CONCERTO_GRAPHICS_RHI_MODULE_BUILD

#include "Concerto/Profiler/Profiler.hpp"

#include <mutex>
#include <memory>
#include <parallel_hashmap/phmap.h>

namespace cct::gfx::rhi
{
	template<typename Key, typename Value>
	using ThreadSafeHashMap = phmap::parallel_flat_hash_map<Key, Value, phmap::priv::hash_default_hash<Key>, phmap::priv::hash_default_eq<Key>, phmap::priv::Allocator<std::pair<const Key, Value>>, 4, std::mutex>;

	template<typename T>
	using ThreadSafeHashSet = phmap::parallel_flat_hash_set<T, phmap::priv::hash_default_hash<T>, phmap::priv::hash_default_eq<T>, phmap::priv::Allocator<T>, 4, std::mutex>;
}

#include <Concerto/Graphics/Core/Defines.hpp>

#define CCT_RHI_LOG_TRACE(fmt, ...) CCT_GFX_LOG_TRACE("RHI", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_RHI_LOG_DEBUG(fmt, ...) CCT_GFX_LOG_DEBUG("RHI", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_RHI_LOG_INFO(fmt, ...) CCT_GFX_LOG_INFO("RHI", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_RHI_LOG_WARN(fmt, ...) CCT_GFX_LOG_WARN("RHI", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_RHI_LOG_ERROR(fmt, ...) CCT_GFX_LOG_ERROR("RHI", fmt __VA_OPT__(, ) __VA_ARGS__)
#define CCT_RHI_LOG_CRITICAL(fmt, ...) CCT_GFX_LOG_CRITICAL("RHI", fmt __VA_OPT__(, ) __VA_ARGS__)

#endif //CONCERTO_GRAPHICS_RHI_DEFINES_HPP