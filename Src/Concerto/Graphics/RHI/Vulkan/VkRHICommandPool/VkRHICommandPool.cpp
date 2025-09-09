//
// Created by arthur on 03/09/2024.
//

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandPool/VkRHICommandPool.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandBuffer/VkRHICommandBuffer.hpp"

namespace cct::gfx::rhi
{
	VkRHICommandPool::VkRHICommandPool(VkRHIDevice& device, QueueFamily family, CommandBufferUsage usage) :
		rhi::CommandPool(usage),
		vk::CommandPool(device, device.GetQueueFamilyIndex(static_cast<vk::Queue::Type>(family))), //fixme?
		m_device(device)
	{
	}

	std::unique_ptr<CommandBuffer> VkRHICommandPool::AllocateCommandBuffer()
	{
		return std::make_unique<VkRHICommandBuffer>(m_device, *this, m_usage);
	}
}