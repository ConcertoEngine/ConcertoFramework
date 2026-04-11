//
// Created by arthur on 03/09/2024.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIBuffer/VkRHIBuffer.hpp"

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"

namespace cct::gfx::rhi
{
	VkRHIBuffer::VkRHIBuffer(VkRHIDevice& device, rhi::BufferUsageFlags usage, UInt32 allocationSize, bool allowBufferMapping) :
		vk::Buffer(device.GetAllocator(), allocationSize, Converters::ToVulkan(usage), VMA_MEMORY_USAGE_AUTO, allowBufferMapping),
		m_device(device),
		m_allowBufferMapping(allowBufferMapping)
	{
	}

	bool VkRHIBuffer::CopyTo(const Texture& texture)
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();

		const auto& vkTexture = cct::Cast<const VkRHITexture&>(texture);
		auto& uploadContext = m_device.GetUploadContext();
		auto* commandBuffer = uploadContext.AcquireSecondaryCommandBuffer();
		if (!commandBuffer)
			return false;

		commandBuffer->Begin();
		{
			VkImageSubresourceRange range;
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			range.baseMipLevel = 0;
			range.levelCount = 1;
			range.baseArrayLayer = 0;
			range.layerCount = 1;

			VkImageMemoryBarrier imageBarrier_toTransfer = {};
			imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

			imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toTransfer.image = *vkTexture.GetImage().Get();
			imageBarrier_toTransfer.subresourceRange = range;

			imageBarrier_toTransfer.srcAccessMask = 0;
			imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			vk::Buffer::m_device->vkCmdPipelineBarrier(*commandBuffer->Get(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;

			copyRegion.imageExtent = {
				.width = vkTexture.GetImage().GetExtent().width,
				.height = vkTexture.GetImage().GetExtent().height,
				.depth = 1
			};;

			vk::Buffer::m_device->vkCmdCopyBufferToImage(*commandBuffer->Get(), *vk::Buffer::Get(), *vkTexture.GetImage().Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

			VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

			imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vk::Buffer::m_device->vkCmdPipelineBarrier(*commandBuffer->Get(), VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
				&imageBarrier_toReadable);
		}
		commandBuffer->End();

		return true;
	}

	bool VkRHIBuffer::Map(Byte** data)
	{
		if (!m_allowBufferMapping)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: buffer mapping is not enabled");
			return false;
		}
		return vk::Buffer::Map(data);;
	}

	void VkRHIBuffer::UnMap()
	{
		if (!m_allowBufferMapping)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: buffer mapping is not enabled");
			return;
		}
		vk::Buffer::UnMap();
	}
}
