//
// Created by arthur on 17/08/2023.
//

#ifndef CONCERTO_GRAPHICS_INCLUDE_VULKAN_HPP_
#define CONCERTO_GRAPHICS_INCLUDE_VULKAN_HPP_

#include <span>
#include <string>
#include <map>

#include "Concerto/Graphics/Core/Version.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"

namespace cct::gfx::vk
{
	class Device;
	class PhysicalDevice;

	struct RendererInfo
	{
		std::string applicationName;
		Version applicationVersion;
		bool useImGUI = false;
		UInt32 width;
		UInt32 height;
	};

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API Vulkan
	{
	public:
		Vulkan(RendererInfo info);
		~Vulkan() = default;

		static Vulkan* GetInstance();

		Device* CreateDevice(rhi::DeviceType type = rhi::DeviceType::Dedicated);

		Instance& GetVkInstance();

	private:
		static Vulkan* m_instance;
		RendererInfo m_info;
		Instance m_vkInstance;
		std::span<PhysicalDevice> m_physicalDevices;
		std::multimap<rhi::DeviceType, Device> m_devices;
	};
}

#endif //CONCERTO_GRAPHICS_INCLUDE_VULKAN_HPP_