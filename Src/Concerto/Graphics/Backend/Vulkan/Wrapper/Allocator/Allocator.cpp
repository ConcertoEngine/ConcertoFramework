//
// Created by arthur on 21/06/2022.
//
#include <cassert>
#include <stdexcept>
#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0


#include <Concerto/Core/Assert.hpp>
#include <vk_mem_alloc.h>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Allocator/Allocator.hpp"

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Buffer/Buffer.hpp"

namespace cct::gfx::vk
{
	Allocator::Allocator(Device& device) :
		Object<VmaAllocator>(device)
	{
		if (Create(device) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	Allocator::~Allocator()
	{
		if (!IsValid())
			return;
		vmaDestroyAllocator(m_handle);
	}

	VkResult Allocator::Create(Device& device)
	{
		m_device = &device;

		PhysicalDevice& physicalDevice = device.GetPhysicalDevice();
		Instance& instance = physicalDevice.GetInstance();

		VmaVulkanFunctions vulkanFunctions{
			.vkGetInstanceProcAddr = Instance::vkGetInstanceProcAddr,
			.vkGetDeviceProcAddr = instance.vkGetDeviceProcAddr,
			.vkGetPhysicalDeviceProperties = instance.vkGetPhysicalDeviceProperties,
			.vkGetPhysicalDeviceMemoryProperties = instance.vkGetPhysicalDeviceMemoryProperties,
			.vkAllocateMemory = device.vkAllocateMemory,
			.vkFreeMemory = device.vkFreeMemory,
			.vkMapMemory = device.vkMapMemory,
			.vkUnmapMemory = device.vkUnmapMemory,
			.vkFlushMappedMemoryRanges = device.vkFlushMappedMemoryRanges,
			.vkInvalidateMappedMemoryRanges = device.vkInvalidateMappedMemoryRanges,
			.vkBindBufferMemory = device.vkBindBufferMemory,
			.vkBindImageMemory = device.vkBindImageMemory,
			.vkGetBufferMemoryRequirements = device.vkGetBufferMemoryRequirements,
			.vkGetImageMemoryRequirements = device.vkGetImageMemoryRequirements,
			.vkCreateBuffer = device.vkCreateBuffer,
			.vkDestroyBuffer = device.vkDestroyBuffer,
			.vkCreateImage = device.vkCreateImage,
			.vkDestroyImage = device.vkDestroyImage,
			.vkCmdCopyBuffer = device.vkCmdCopyBuffer,
			.vkGetBufferMemoryRequirements2KHR = nullptr,
			.vkGetImageMemoryRequirements2KHR = nullptr,
			.vkBindBufferMemory2KHR = nullptr,
			.vkBindImageMemory2KHR = nullptr,
			.vkGetPhysicalDeviceMemoryProperties2KHR = nullptr,
			.vkGetDeviceBufferMemoryRequirements = nullptr,
			.vkGetDeviceImageMemoryRequirements = nullptr,
			.vkGetMemoryWin32HandleKHR = nullptr
		};

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = *physicalDevice.Get();
		allocatorInfo.device = *device.Get();
		allocatorInfo.instance = *instance.Get();
		allocatorInfo.pVulkanFunctions = &vulkanFunctions;

		m_lastResult = vmaCreateAllocator(&allocatorInfo, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "Unable to create VmaAllocator VkResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}
}
