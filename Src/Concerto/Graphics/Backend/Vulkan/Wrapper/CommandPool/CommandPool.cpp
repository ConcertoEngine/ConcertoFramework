//
// Created by arthur on 14/06/22.
//

#include <stdexcept>
#include <iostream>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandPool/CommandPool.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{
	CommandPool::CommandPool(Device& device, UInt32 queueFamily) :
		Object(device),
		m_queueFamily(queueFamily)
	{
		VkCommandPoolCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		info.queueFamilyIndex = m_queueFamily;
		m_lastResult = m_device->vkCreateCommandPool(*m_device->Get(), &info, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "Error cannot create command pool: VkResult={}", static_cast<int>(m_lastResult));
	}

	CommandPool::~CommandPool()
	{
		if (!IsValid())
			return;
		m_device->vkDestroyCommandPool(*m_device->Get(), m_handle, nullptr);
	}

	void CommandPool::Reset() const
	{
		m_lastResult = m_device->vkResetCommandPool(*m_device->Get(), m_handle, 0);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: Error cannot reset command pool: VkResult={}", static_cast<int>(m_lastResult));
	}

	vk::CommandBuffer CommandPool::AllocateCommandBuffer(VkCommandBufferLevel level)
	{
		return vk::CommandBuffer(*this, level);
	}
}
