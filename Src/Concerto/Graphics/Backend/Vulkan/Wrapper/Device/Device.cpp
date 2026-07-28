//
// Created by arthur on 25/10/2022.
//

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

#include <stdexcept>
#include <unordered_set>
#include <volk.h> // must be under this ^ include

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ObjectDebug/ObjectDebug.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"

namespace cct::gfx::vk
{
	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
#ifdef CCT_PLATFORM_WINDOWS
		VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
		VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
#endif
	};

	Device::Device(PhysicalDevice& physicalDevice) :
		m_physicalDevice(&physicalDevice),
		m_allocator(nullptr)
	{
		auto lastError = Create(physicalDevice);
		if (lastError != VK_SUCCESS)
			throw VkException(lastError);
	}

	Device::~Device()
	{
		if (IsValid())
		{
			// Ensure all GPU operations are complete
			vkDeviceWaitIdle(m_handle);
		}

		// Explicitly destroy the allocator before vkDestroyDevice
		// This ensures all GPU memory allocations are freed in the correct order
		m_allocator.reset();

		// Clear other resources before device destruction
		m_queues.clear();
		m_extensions.clear();

		vkDestroyDevice(m_handle, nullptr);
	}

	Device::Device(Device&& other) noexcept
	{
		m_physicalDevice = std::exchange(other.m_physicalDevice, {});
		m_allocator = std::exchange(other.m_allocator, {});
		m_queues = std::exchange(other.m_queues, {});
		m_extensions = std::exchange(other.m_extensions, {});

#define CONCERTO_VULKAN_BACKEND_DEVICE_FUNCTION(func) this->func = std::exchange(other.func, nullptr);
#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_BEGIN(ext)
#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_FUNCTION(func, ...) this->func = std::exchange(other.func, nullptr);
#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_END

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/DeviceFunction.hpp"
	}

	Device& Device::operator=(Device&& other) noexcept
	{
		std::swap(m_physicalDevice, other.m_physicalDevice);
		std::swap(m_allocator, other.m_allocator);
		std::swap(m_queues, other.m_queues);
		std::swap(m_extensions, other.m_extensions);

#define CONCERTO_VULKAN_BACKEND_DEVICE_FUNCTION(func) std::swap(func, other.func);
#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_BEGIN(ext)
#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_FUNCTION(func, ...) std::swap(func, other.func);
#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_END

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/DeviceFunction.hpp"

		return *this;
	}

	VkResult Device::Create(PhysicalDevice& physicalDevice)
	{
		std::span<VkQueueFamilyProperties> queueFamilyProperties = m_physicalDevice->GetQueueFamilyProperties();
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(queueFamilyProperties.size());

		float queuePriority = 1.0f;
		for (UInt32 i = 0; i < static_cast<UInt32>(queueFamilyProperties.size()); ++i)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = i;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.flags = 0;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		// Only request extensions the physical device actually supports (allows headless use).
		auto supportedNames = physicalDevice.GetExtensionPropertiesNames();
		std::unordered_set<std::string_view> supportedSet(supportedNames.begin(), supportedNames.end());
		std::vector<const char*> enabledExtensions;
		enabledExtensions.reserve(deviceExtensions.size() + 3);
		for (const char* ext : deviceExtensions)
		{
			if (supportedSet.contains(ext))
			{
				enabledExtensions.push_back(ext);
			}
		}

		bool videoDecode = false;
#if defined(VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME)
		{
			const char* videoExts[] = {
				VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
				VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME,
				VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME,
			};
			videoDecode = physicalDevice.GetProperties().apiVersion >= VK_API_VERSION_1_3;
			for (const char* ext : videoExts)
				videoDecode = videoDecode && supportedSet.contains(ext);
			if (videoDecode)
			{
				for (const char* ext : videoExts)
					enabledExtensions.push_back(ext);
			}
		}
#endif

		// shaderDrawParameters was always required. The video path additionally needs
		// timeline semaphores + synchronization2 + sampler ycbcr conversion (FFmpeg's
		// Vulkan decoder + our NV12 conversion pass rely on them). We enable only the
		// bits the device reports as available.
		VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParams{};
		shaderDrawParams.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
		shaderDrawParams.shaderDrawParameters = VK_TRUE;

		VkPhysicalDeviceVulkan11Features vk11{};
		vk11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		VkPhysicalDeviceVulkan12Features vk12{};
		vk12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		VkPhysicalDeviceVulkan13Features vk13{};
		vk13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<UInt32>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<UInt32>(enabledExtensions.size());
		createInfo.ppEnabledExtensionNames = enabledExtensions.data();

		if (videoDecode)
		{
			VkPhysicalDeviceVulkan13Features avail13{};
			avail13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
			VkPhysicalDeviceVulkan12Features avail12{};
			avail12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
			avail12.pNext = &avail13;
			VkPhysicalDeviceVulkan11Features avail11{};
			avail11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
			avail11.pNext = &avail12;
			VkPhysicalDeviceFeatures2 avail2{};
			avail2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			avail2.pNext = &avail11;
			physicalDevice.GetInstance().vkGetPhysicalDeviceFeatures2(*physicalDevice.Get(), &avail2);

			vk11.shaderDrawParameters = VK_TRUE; // subsumes the standalone struct
			vk11.samplerYcbcrConversion = avail11.samplerYcbcrConversion;
			vk12.timelineSemaphore = avail12.timelineSemaphore;
			vk13.synchronization2 = avail13.synchronization2;

			vk11.pNext = &vk12;
			vk12.pNext = &vk13;
			createInfo.pNext = &vk11;
		}
		else
		{
			createInfo.pNext = &shaderDrawParams;
		}

		const VkResult result = physicalDevice.GetInstance().vkCreateDevice(*m_physicalDevice->Get(), &createInfo, nullptr, &m_handle);
		CCT_ASSERT(result == VK_SUCCESS, "Error cannot create logical device: VkResult={}", static_cast<int>(result));
		if (result != VK_SUCCESS)
			return result;

		for (auto& ext : enabledExtensions)
			m_extensions.emplace(ext);

		m_videoDecodeSupported = videoDecode;
		if (videoDecode)
		{
			constexpr UInt32 videoDecodeBit = 0x00000020;
			for (UInt32 i = 0; i < static_cast<UInt32>(queueFamilyProperties.size()); ++i)
			{
				if (queueFamilyProperties[i].queueFlags & videoDecodeBit)
				{
					m_videoDecodeQueueFamily = i;
					break;
				}
			}
		}

		VolkDeviceTable deviceTable;
		volkLoadDeviceTable(&deviceTable, m_handle);
#define CONCERTO_VULKAN_BACKEND_DEVICE_FUNCTION(func) this->func = deviceTable.func;

#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_BEGIN(ext) \
	if (IsExtensionEnabled(#ext))                     \
	{
#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_FUNCTION(func, ...)                          \
	CONCERTO_VULKAN_BACKEND_DEVICE_FUNCTION(func)                                       \
	if (this->func == nullptr)                                                          \
	{                                                                                   \
		CCT_ASSERT_FALSE("ConcertoGraphics: Function: " #func                           \
						 " is null but the extension has been reported has supported"); \
	}
#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_END }

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/DeviceFunction.hpp"

		CreateAllocator();

		return VK_SUCCESS;
	}

	UInt32 Device::GetQueueFamilyIndex(Queue::Type queueType) const
	{
		const std::span<VkQueueFamilyProperties> queueFamilyProperties = m_physicalDevice->GetQueueFamilyProperties();
		UInt32 i = 0;
		for (const VkQueueFamilyProperties& properties : queueFamilyProperties)
		{
			if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueType == Queue::Type::Graphics)
				return i;
			else if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT && queueType == Queue::Type::Compute)
				return i;
			else if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT && queueType == Queue::Type::Transfer)
				return i;
			i++;
		}
		CCT_ASSERT_FALSE("No queue family found");
		return std::numeric_limits<UInt32>::max();
	}

	UInt32 Device::GetQueueFamilyIndex(UInt32 flag) const
	{
		const std::span<VkQueueFamilyProperties> queueFamilyProperties = m_physicalDevice->GetQueueFamilyProperties();
		UInt32 fallback = std::numeric_limits<UInt32>::max();
		UInt32 i = 0;
		for (const VkQueueFamilyProperties properties : queueFamilyProperties)
		{
			if (properties.queueFlags & flag)
			{
				if (!(flag & VK_QUEUE_GRAPHICS_BIT) && (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT))
				{
					if (fallback == std::numeric_limits<UInt32>::max())
						fallback = i;
				}
				else
				{
					return i;
				}
			}
			++i;
		}
		if (fallback != std::numeric_limits<UInt32>::max())
			return fallback;
		CCT_ASSERT_FALSE("No queue family found");
		return std::numeric_limits<UInt32>::max();
	}

	Queue& Device::GetQueue(Queue::Type queueType)
	{
		auto it = m_queues.find(queueType);
		if (it != m_queues.end())
			return it->second;
		auto emplace = m_queues.emplace(queueType, Queue(*this, GetQueueFamilyIndex(queueType)));
		return emplace.first->second;
	}

	void Device::WaitIdle() const
	{
		const VkResult result = this->vkDeviceWaitIdle(m_handle);
		CCT_ASSERT(result == VK_SUCCESS, "ConcertoGraphics: Failed to Wait for device idle VkResult={}", static_cast<int>(result));
	}

	void Device::UpdateDescriptorSetsWrite(std::span<VkWriteDescriptorSet> descriptorWrites) const
	{
		this->vkUpdateDescriptorSets(m_handle, static_cast<UInt32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void Device::UpdateDescriptorSetWrite(const VkWriteDescriptorSet& descriptorWrite) const
	{
		this->vkUpdateDescriptorSets(m_handle, 1, &descriptorWrite, 0, nullptr);
	}

	PhysicalDevice& Device::GetPhysicalDevice() const
	{
		CCT_ASSERT(m_physicalDevice, "ConcertoGraphics: Invalid physical device handle");
		return *m_physicalDevice;
	}

	Allocator& Device::GetAllocator() const
	{
		CCT_ASSERT(m_allocator != nullptr, "ConcertoGraphics: Allocator handle is null");
		return *m_allocator;
	}

	Instance& Device::GetInstance() const
	{
		CCT_ASSERT(m_physicalDevice, "ConcertoGraphics: Invalid physical device.");
		return m_physicalDevice->GetInstance();
	}

	bool Device::IsExtensionEnabled(const std::string& ext) const
	{
		return m_extensions.contains(ext);
	}

	void Device::CreateAllocator()
	{
		m_allocator = std::make_unique<Allocator>(*this);
		CCT_ASSERT(m_allocator != nullptr, "ConcertoGraphics: Cannot create allocator");
	}

} // namespace cct::gfx::vk