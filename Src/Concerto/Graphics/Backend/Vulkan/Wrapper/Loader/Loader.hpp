//
// Created by arthur on 11/09/2026.
//

#ifndef CONCERTO_GRAPHICS_VULKAN_LOADER_HPP
#define CONCERTO_GRAPHICS_VULKAN_LOADER_HPP

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"

extern CONCERTO_GRAPHICS_VULKAN_BACKEND_API PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
extern CONCERTO_GRAPHICS_VULKAN_BACKEND_API PFN_vkCreateInstance vkCreateInstance;
extern CONCERTO_GRAPHICS_VULKAN_BACKEND_API PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
extern CONCERTO_GRAPHICS_VULKAN_BACKEND_API PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
extern CONCERTO_GRAPHICS_VULKAN_BACKEND_API PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;

namespace cct::gfx::vk::Loader
{
	CONCERTO_GRAPHICS_VULKAN_BACKEND_API VkResult Initialize();
} // namespace cct::gfx::vk::Loader

#endif // CONCERTO_GRAPHICS_VULKAN_LOADER_HPP
