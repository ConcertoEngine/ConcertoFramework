//
// Created by arthur on 09/04/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIQueue/VkRHIQueue.hpp"

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandBuffer/VkRHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIFence/VkRHIFence.hpp"

namespace cct::gfx::rhi
{
	VkRHIQueue::VkRHIQueue(vk::Queue& queue, VkRHIDevice& device) :
		m_queue(&queue),
		m_device(&device)
	{
	}

	void VkRHIQueue::Submit(CommandBuffer& cmdBuf, Fence* fence)
	{
		const auto& vkCmdBuf = Cast<const VkRHICommandBuffer&>(cmdBuf);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkCommandBuffer cmdBufHandle = *static_cast<const vk::CommandBuffer&>(vkCmdBuf).Get();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBufHandle;

		VkFence vkFence = VK_NULL_HANDLE;
		if (fence)
		{
			auto& vkRHIFence = Cast<VkRHIFence&>(*fence);
			vkFence = *vkRHIFence.GetFence().Get();
		}

		VkResult result = m_device->vkQueueSubmit(*m_queue->Get(), 1, &submitInfo, vkFence);
		CCT_ASSERT(result == VK_SUCCESS, "ConcertoGraphics: vkQueueSubmit failed VkResult={}", static_cast<int>(result));
	}
} // namespace cct::gfx::rhi
