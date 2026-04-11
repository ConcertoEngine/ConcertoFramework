//
// Created by arthur on 25/10/2022.
//

#include <stdexcept>
#include <vector>
#include <utility>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"

namespace cct::gfx::vk
{
	PhysicalDevice::PhysicalDevice() :
		m_instance(nullptr)
	{
	}

	PhysicalDevice::PhysicalDevice(vk::Instance& instance, VkPhysicalDevice physicalDevice) :
		m_physicalDevice(physicalDevice),
		m_instance(&instance)
	{

	}

	std::span<VkQueueFamilyProperties> PhysicalDevice::GetQueueFamilyProperties() const
	{
		if (m_queueFamilyProperties)
			return m_queueFamilyProperties.value();
		UInt32 queueFamilyCount = 0;
		m_instance->vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		m_instance->vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
		m_queueFamilyProperties = std::move(queueFamilyProperties);
		return m_queueFamilyProperties.value();
	}

	PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
	{
		m_queueFamilyProperties = std::exchange(other.m_queueFamilyProperties, std::nullopt);
		m_physicalDeviceProperties = std::exchange(other.m_physicalDeviceProperties, std::nullopt);
		m_physicalDeviceFeatures = std::exchange(other.m_physicalDeviceFeatures, std::nullopt);
		m_physicalDeviceMemoryProperties = std::exchange(other.m_physicalDeviceMemoryProperties, std::nullopt);
		m_extensionProperties = std::exchange(other.m_extensionProperties, std::nullopt);
		m_extensionPropertiesNames = std::exchange(other.m_extensionPropertiesNames, std::nullopt);
		m_formatProperties = std::exchange(other.m_formatProperties, std::nullopt);
		m_capabilities = std::exchange(other.m_capabilities, std::nullopt);
		m_formats = std::exchange(other.m_formats, std::nullopt);
		m_presentModes = std::exchange(other.m_presentModes, std::nullopt);
		m_physicalDevice = std::exchange(other.m_physicalDevice, nullptr);
		m_instance = other.m_instance;
	}

	PhysicalDevice& PhysicalDevice::operator=(PhysicalDevice&& other) noexcept
	{
		std::swap(m_queueFamilyProperties, other.m_queueFamilyProperties);
		std::swap(m_physicalDeviceProperties, other.m_physicalDeviceProperties);
		std::swap(m_physicalDeviceFeatures, other.m_physicalDeviceFeatures);
		std::swap(m_physicalDeviceMemoryProperties, other.m_physicalDeviceMemoryProperties);
		std::swap(m_extensionProperties, other.m_extensionProperties);
		std::swap(m_extensionPropertiesNames, other.m_extensionPropertiesNames);
		std::swap(m_formatProperties, other.m_formatProperties);
		std::swap(m_capabilities, other.m_capabilities);
		std::swap(m_formats, other.m_formats);
		std::swap(m_presentModes, other.m_presentModes);
		std::swap(m_physicalDevice, other.m_physicalDevice);
		std::swap(m_instance,  other.m_instance);
		return *this;
	}

	VkResult PhysicalDevice::Create(Instance& instance, VkPhysicalDevice physicalDevice)
	{
		m_physicalDevice = physicalDevice;
		m_instance = &instance;

		return VK_SUCCESS;
	}

	VkPhysicalDeviceProperties PhysicalDevice::GetProperties() const
	{
		if (m_physicalDeviceProperties)
			return m_physicalDeviceProperties.value();
		VkPhysicalDeviceProperties properties;
		m_instance->vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
		m_physicalDeviceProperties = properties;
		return m_physicalDeviceProperties.value();
	}

	VkPhysicalDeviceFeatures PhysicalDevice::GetFeatures() const
	{
		if (m_physicalDeviceFeatures)
			return m_physicalDeviceFeatures.value();
		VkPhysicalDeviceFeatures features;
		m_instance->vkGetPhysicalDeviceFeatures(m_physicalDevice, &features);
		m_physicalDeviceFeatures = features;
		return m_physicalDeviceFeatures.value();
	}

	VkPhysicalDeviceMemoryProperties PhysicalDevice::GetMemoryProperties() const
	{
		if (m_physicalDeviceMemoryProperties)
			return m_physicalDeviceMemoryProperties.value();
		VkPhysicalDeviceMemoryProperties memoryProperties;
		m_instance->vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryProperties);
		m_physicalDeviceMemoryProperties = memoryProperties;
		return m_physicalDeviceMemoryProperties.value();
	}

	std::span<VkExtensionProperties> PhysicalDevice::GetExtensionProperties() const
	{
		if (m_extensionProperties)
			return m_extensionProperties.value();
		UInt32 extensionCount = 0;
		m_instance->vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		m_instance->vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, extensions.data());
		m_extensionProperties = extensions;
		return m_extensionProperties.value();
	}

	VkPhysicalDevice* PhysicalDevice::Get()
	{
		CCT_ASSERT(m_physicalDevice != VK_NULL_HANDLE, "ConcertoGraphics: physical device handle is null");
		return &m_physicalDevice;
	}

	VkPhysicalDevice PhysicalDevice::Get() const
	{
		return m_physicalDevice;
	}

	Instance& PhysicalDevice::GetInstance() const
	{
		CCT_ASSERT(m_instance != VK_NULL_HANDLE, "ConcertoGraphics: instance handle is null");
		return *m_instance;
	}

	std::span<const char*> PhysicalDevice::GetExtensionPropertiesNames() const
	{
		if (m_extensionPropertiesNames)
			return m_extensionPropertiesNames.value();
		std::span<VkExtensionProperties> extensions = GetExtensionProperties();
		std::vector<const char*> extensionsNames;
		extensionsNames.reserve(extensions.size());
		for (const auto& extension : extensions)
			extensionsNames.push_back(extension.extensionName);
		m_extensionPropertiesNames = extensionsNames;
		return m_extensionPropertiesNames.value();
	}

	const std::unordered_map<VkFormat, VkFormatProperties>& PhysicalDevice::GetFormatProperties() const
	{
		if (m_formatProperties)
			return m_formatProperties.value();
		m_formatProperties.emplace();
		using VkFormatUnderlyingType = std::underlying_type_t<VkFormat>;

		constexpr VkFormatUnderlyingType firstFormat = VK_FORMAT_R4G4_UNORM_PACK8;
		constexpr VkFormatUnderlyingType lastFormat = VK_FORMAT_ASTC_12x12_SRGB_BLOCK;

		for (VkFormatUnderlyingType format = firstFormat; format < lastFormat; format++)
		{
			VkFormatProperties formatProperties = {};
			m_instance->vkGetPhysicalDeviceFormatProperties(m_physicalDevice, static_cast<VkFormat>(format), &formatProperties);
			m_formatProperties->emplace(static_cast<VkFormat>(format), formatProperties);
		}
		return m_formatProperties.value();
	}

	VkSurfaceCapabilitiesKHR PhysicalDevice::GetCapabilities(VkSurfaceKHR surface) const
	{
		if (m_capabilities)
			return m_capabilities.value();
		VkSurfaceCapabilitiesKHR capabilities;
		const VkResult result = m_instance->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, surface, &capabilities);
		CCT_ASSERT(result == VK_SUCCESS, "ConcertoGraphics: vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed VKResult={}", static_cast<int>(result));
		m_capabilities = capabilities;
		return m_capabilities.value();
	}

	std::span<VkSurfaceFormatKHR> PhysicalDevice::GetFormats(VkSurfaceKHR surface) const
	{
		if (m_formats)
			return m_formats.value();
		UInt32 formatCount = 0;
		m_instance->vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &formatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> formats(formatCount);
		m_instance->vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &formatCount, formats.data());
		m_formats = formats;
		return m_formats.value();
	}

	std::span<VkPresentModeKHR> PhysicalDevice::GetPresentModes(VkSurfaceKHR surface) const
	{
		if (m_presentModes)
			return m_presentModes.value();
		UInt32 count = 0;
		m_instance->vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &count, nullptr);
		std::vector<VkPresentModeKHR> modes(count);
		m_instance->vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &count, modes.data());
		m_presentModes = modes;
		return m_presentModes.value();
	}

	PhysicalDevice::SurfaceSupportDetails PhysicalDevice::GetSurfaceSupportDetails(VkSurfaceKHR surface) const
	{
		SurfaceSupportDetails details;
		details.capabilities = GetCapabilities(surface);
		details.formats = GetFormats(surface);
		details.presentModes = GetPresentModes(surface);
		return details;
	}
} // cct::gfx::vk