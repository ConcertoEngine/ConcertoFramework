//
// Created by arthur on 11/09/2026.
//

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Loader/Loader.hpp"

#include <array>

#include <Concerto/Core/Assert.hpp>
#include <Concerto/Core/DynLib/DynLib.hpp>

PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
PFN_vkCreateInstance vkCreateInstance = nullptr;
PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = nullptr;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = nullptr;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = nullptr;

namespace cct::gfx::vk::Loader
{
	namespace
	{
		cct::DynLib g_vulkanLib;

		bool LoadVulkanLibrary()
		{
#ifdef CCT_PLATFORM_WINDOWS
			constexpr std::array candidates = { "vulkan-1.dll" };
#elif defined(CCT_PLATFORM_MACOS)
			constexpr std::array candidates = { "libvulkan.dylib", "libvulkan.1.dylib", "libMoltenVK.dylib" };
#else
			constexpr std::array candidates = { "libvulkan.so.1", "libvulkan.so" };
#endif
			for (const char* candidate : candidates)
			{
				if (g_vulkanLib.Load(candidate))
					return true;
			}
			return false;
		}
	} // namespace

	VkResult Initialize()
	{
		if (vkGetInstanceProcAddr != nullptr)
			return VK_SUCCESS;

		if (!LoadVulkanLibrary())
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't find the Vulkan loader library on this system");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(g_vulkanLib.GetSymbol("vkGetInstanceProcAddr"));
		if (vkGetInstanceProcAddr == nullptr)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't resolve vkGetInstanceProcAddr from the Vulkan loader library");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(vkGetInstanceProcAddr(nullptr, "vkCreateInstance"));
		vkEnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
		vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties"));
		vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));

		if (vkCreateInstance == nullptr)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Couldn't resolve vkCreateInstance from the Vulkan loader library");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		UInt32 instanceVersion = VK_API_VERSION_1_0;
		if (vkEnumerateInstanceVersion == nullptr || vkEnumerateInstanceVersion(&instanceVersion) != VK_SUCCESS)
			instanceVersion = VK_API_VERSION_1_0;

		if (instanceVersion < VK_API_VERSION_1_4)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: system Vulkan loader reports API version {}.{}.{}, but ConcertoGraphics requires Vulkan >= 1.4",
							 VK_API_VERSION_MAJOR(instanceVersion), VK_API_VERSION_MINOR(instanceVersion), VK_API_VERSION_PATCH(instanceVersion));
			return VK_ERROR_INCOMPATIBLE_DRIVER;
		}

		return VK_SUCCESS;
	}
} // namespace cct::gfx::vk::Loader
