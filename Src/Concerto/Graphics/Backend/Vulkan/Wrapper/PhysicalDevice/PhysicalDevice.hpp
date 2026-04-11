//
// Created by arthur on 25/10/2022.
//

#ifndef CONCERTO_GRAPHICS_PHYSICALDEVICE_HPP
#define CONCERTO_GRAPHICS_PHYSICALDEVICE_HPP

#include <span>
#include <optional>
#include <vector>
#include <unordered_map>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"

namespace cct::gfx::vk
{
	class Instance;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API PhysicalDevice
	{
	public:
		struct SurfaceSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities{};
			std::span<VkSurfaceFormatKHR> formats{};
			std::span<VkPresentModeKHR> presentModes{};
		};

		PhysicalDevice();
		PhysicalDevice(Instance& instance, VkPhysicalDevice physicalDevice);
		~PhysicalDevice() = default;
		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice(PhysicalDevice&&) noexcept;

		PhysicalDevice& operator=(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(PhysicalDevice&&) noexcept;

		VkResult Create(Instance& instance, VkPhysicalDevice physicalDevice);

		[[nodiscard]] std::span<VkQueueFamilyProperties> GetQueueFamilyProperties() const;
		[[nodiscard]] VkPhysicalDeviceProperties GetProperties() const;
		[[nodiscard]] VkPhysicalDeviceFeatures GetFeatures() const;
		[[nodiscard]] VkPhysicalDeviceMemoryProperties GetMemoryProperties() const;
		[[nodiscard]] std::span<VkExtensionProperties> GetExtensionProperties() const;
		[[nodiscard]] std::span<const char*> GetExtensionPropertiesNames() const;
		[[nodiscard]] const std::unordered_map<VkFormat, VkFormatProperties>& GetFormatProperties() const;
		[[nodiscard]] VkSurfaceCapabilitiesKHR GetCapabilities(VkSurfaceKHR surface) const;
		[[nodiscard]] std::span<VkSurfaceFormatKHR> GetFormats(VkSurfaceKHR surface) const;
		[[nodiscard]] std::span<VkPresentModeKHR> GetPresentModes(VkSurfaceKHR surface) const;
		[[nodiscard]] SurfaceSupportDetails GetSurfaceSupportDetails(VkSurfaceKHR surface) const;

		[[nodiscard]] VkPhysicalDevice* Get();
		[[nodiscard]] VkPhysicalDevice Get() const;
		[[nodiscard]] Instance& GetInstance() const;

	private:
		mutable std::optional<std::vector<VkQueueFamilyProperties>> m_queueFamilyProperties;
		mutable std::optional<VkPhysicalDeviceProperties> m_physicalDeviceProperties;
		mutable std::optional<VkPhysicalDeviceFeatures> m_physicalDeviceFeatures;
		mutable std::optional<VkPhysicalDeviceMemoryProperties> m_physicalDeviceMemoryProperties;
		mutable std::optional<std::vector<VkExtensionProperties>> m_extensionProperties;
		mutable std::optional<std::vector<const char*>> m_extensionPropertiesNames;
		mutable std::optional<std::unordered_map<VkFormat, VkFormatProperties>> m_formatProperties;
		mutable std::optional<VkSurfaceCapabilitiesKHR> m_capabilities;
		mutable std::optional<std::vector<VkSurfaceFormatKHR>> m_formats;
		mutable std::optional<std::vector<VkPresentModeKHR>> m_presentModes;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		vk::Instance* m_instance;
};

} // cct::gfx::vk

#endif //CONCERTO_GRAPHICS_PHYSICALDEVICE_HPP
