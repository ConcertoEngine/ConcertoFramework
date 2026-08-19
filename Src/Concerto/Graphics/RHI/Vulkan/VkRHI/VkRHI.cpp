//
// Created by arthur on 12/05/2024.
//
#include "Concerto/Graphics/RHI/Vulkan/VkRHI/VkRHI.hpp"

#include <vector>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"

namespace cct::gfx
{
	bool VkRHI::Create(EnumFlags<rhi::ValidationFlags> validationFlags)
	{
		std::vector<const char*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
#ifdef VK_USE_PLATFORM_XCB_KHR
		extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
		extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
		extensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
		extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef VK_USE_PLATFORM_METAL_EXT
		extensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#endif
		std::vector<const char*> layers;
		std::vector<VkValidationFeatureEnableEXT> validationFeatures;
		if (validationFlags.Any())
		{
			layers.push_back("VK_LAYER_KHRONOS_validation");
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
		if (validationFlags.Contains(rhi::ValidationFlags::SyncValidation))
			validationFeatures.push_back(VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);
		if (validationFlags.Contains(rhi::ValidationFlags::BestPractices))
			validationFeatures.push_back(VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
		if (validationFlags.Contains(rhi::ValidationFlags::GpuAssisted))
			validationFeatures.push_back(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT);
		if (validationFlags.Contains(rhi::ValidationFlags::DebugPrintf))
			validationFeatures.push_back(VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT);
		if (!validationFeatures.empty())
			extensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);

		m_instance = std::make_unique<vk::Instance>("", "", Version{1, 3, 0}, Version{}, Version{}, extensions, layers, validationFeatures);
		if (m_instance->GetLastResult() != VK_SUCCESS)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Failed to initialize Vulkan instance, VkResult={}", static_cast<Int32>(m_instance->GetLastResult()));
			return false;
		}
		return true;
	}

	std::span<const rhi::DeviceInfo> VkRHI::EnumerateDevices()
	{
		if (!m_instance)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Invalid Vulkan instance, please call VkRHI::Create before");
			return m_devicesInfo;
		}
		const auto devices = m_instance->EnumeratePhysicalDevices();
		for (auto& device : devices)
		{
			m_devicesInfo.emplace_back(
				device.GetProperties().deviceName,
				static_cast<rhi::VendorId>(device.GetProperties().vendorID),
				FromVulkan(device.GetProperties().deviceType));
		}
		return m_devicesInfo;
	}

	std::unique_ptr<rhi::Device> VkRHI::CreateDevice(std::size_t index)
	{
		CCT_ASSERT(index <= m_devicesInfo.size(), "ConcertoGraphics: Device index out of range");
		if (!m_instance)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Invalid Vulkan instance, please call VkRHI::Create before");
			return nullptr;
		}
		std::unique_ptr<rhi::Device> device;
		const auto physicalDevices = m_instance->EnumeratePhysicalDevices();
		for (std::size_t i = 0; i < physicalDevices.size(); ++i)
		{
			if (i != index)
				continue;
			device = std::make_unique<rhi::VkRHIDevice>(physicalDevices[i]);
		}
		CCT_ASSERT(device, "ConcertoGraphics: Unable to create Vulkan device");
		return device;
	}

	constexpr rhi::DeviceType VkRHI::FromVulkan(VkPhysicalDeviceType deviceType)
	{
		switch (deviceType)
		{
			case VK_PHYSICAL_DEVICE_TYPE_OTHER:
				return rhi::DeviceType::Other;
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				return rhi::DeviceType::Integrated;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				return rhi::DeviceType::Dedicated;
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				return rhi::DeviceType::Virtual;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				return rhi::DeviceType::Software;
			default:
				return {};
		}
	}

	void VkRHI::SetLogger(Logger& logger)
	{
		Logger::SetContext(&logger);
		vk::Instance::SetLogger(logger);
	}
} // namespace cct::gfx
