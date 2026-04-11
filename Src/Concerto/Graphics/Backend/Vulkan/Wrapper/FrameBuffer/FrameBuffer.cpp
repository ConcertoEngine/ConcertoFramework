//
// Created by arthur on 12/06/22.
//

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/FrameBuffer/FrameBuffer.hpp"

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/RenderPass/RenderPass.hpp"

namespace cct::gfx::vk
{
	FrameBuffer::FrameBuffer() :
		m_extent()
	{
	}

	FrameBuffer::FrameBuffer(const RenderPass& renderPass, const std::vector<VkImageView>& attachments, VkExtent2D extent) :
		Object(*renderPass.GetDevice()),
		m_extent(extent)
	{
		if (Create(renderPass, attachments, extent) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	FrameBuffer::~FrameBuffer()
	{
		if (!IsValid())
			return;
		m_device->vkDestroyFramebuffer(*m_device->Get(), m_handle, nullptr);
	}

	VkResult FrameBuffer::Create(const RenderPass& renderPass, const std::vector<VkImageView>& attachments, VkExtent2D extent)
	{
		m_device = renderPass.GetDevice();
		m_extent = extent;

		VkFramebufferCreateInfo frameBufferCreateInfo = VulkanInitializer::FramebufferCreateInfo(*renderPass.Get(), extent);

		frameBufferCreateInfo.pAttachments = attachments.data();
		frameBufferCreateInfo.attachmentCount = static_cast<UInt32>(attachments.size());
		m_lastResult = m_device->vkCreateFramebuffer(*m_device->Get(), &frameBufferCreateInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateFramebuffer failed VkResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}

	VkExtent2D FrameBuffer::GetExtent2D() const
	{
		return m_extent;
	}
} // cct::gfx::vk