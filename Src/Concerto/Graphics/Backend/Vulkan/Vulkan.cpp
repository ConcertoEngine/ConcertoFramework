//
// Created by arthur on 17/08/2023.
//

#include <array>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Vulkan.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{
	namespace
	{

#if defined(CCT_PLATFORM_WINDOWS)
		std::array extensions = { "VK_KHR_surface", "VK_KHR_win32_surface", "VK_EXT_debug_report" };
#elif defined(CCT_PLATFORM_POSIX)
		std::array extensions = { "VK_KHR_surface", "VK_KHR_xcb_surface", "VK_KHR_xlib_surface", "VK_KHR_wayland_surface" };
#endif
		std::array layers = { "VK_LAYER_KHRONOS_validation" /*, "VK_LAYER_LUNARG_api_dump"*/ };
	}


	Vulkan* Vulkan::m_instance = nullptr;

	Vulkan::Vulkan(RendererInfo info) :
		m_info(std::move(info)),
		m_vkInstance(m_info.applicationName,
			m_info.applicationName,
			{ 1, 3, 0 },
			m_info.applicationVersion,
			{ 1, 0, 0 }, extensions, layers),
		m_physicalDevices(m_vkInstance.EnumeratePhysicalDevices())
	{
		CCT_ASSERT(m_instance == nullptr, "Only one renderer can be created");
		m_instance = this;
	}

	Vulkan* Vulkan::GetInstance()
	{
		CCT_ASSERT(m_instance != nullptr, "You must call the constructor of class 'Vulkan', before calling 'GetInstance'");
		return m_instance;
	}

	Device* Vulkan::CreateDevice(rhi::DeviceType type)
	{
		auto it = m_devices.find(type);
		if (it != m_devices.end())
			return &it->second;
		for (auto& physicalDevice : m_physicalDevices)
		{
			auto properties = physicalDevice.GetProperties();
			rhi::DeviceType deviceType = static_cast<rhi::DeviceType>(properties.deviceType);
			if (deviceType != type)
				continue;
			it = m_devices.emplace(deviceType, Device(physicalDevice));
			return &it->second;
		}
		return nullptr;
	}

	Instance& Vulkan::GetVkInstance()
	{
		return m_vkInstance;
	}
}