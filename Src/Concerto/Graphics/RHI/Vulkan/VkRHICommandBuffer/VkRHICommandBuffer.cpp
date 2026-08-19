//
// Created by arthur on 03/09/2024.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandBuffer/VkRHICommandBuffer.hpp"

#include <algorithm>

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Pipeline/Pipeline.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkMaterial.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIBuffer/VkRHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSet/VkRHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIFrameBuffer/VKRHIFrameBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipeline/VkRHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipelineLayout/VkRHIPipelineLayout.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIRenderPass/VkRHIRenderPass.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHISwapChain/VkRHISwapChain.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"

namespace cct::gfx::rhi
{
	VkRHICommandBuffer::VkRHICommandBuffer(VkRHIDevice& device, vk::CommandPool& commandPool, CommandBufferUsage usage) :
		vk::CommandBuffer(commandPool.AllocateCommandBuffer(usage == CommandBufferUsage::Primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY)),
		m_device(&device)
	{
	}

	void VkRHICommandBuffer::Begin()
	{
		vk::CommandBuffer::Begin();
	}

	void VkRHICommandBuffer::End()
	{
		vk::CommandBuffer::End();
	}

	void VkRHICommandBuffer::Reset()
	{
		vk::CommandBuffer::Reset();
	}

	void VkRHICommandBuffer::SetViewport(const Viewport& viewport)
	{
		VkViewport vkViewport = {
			.x = viewport.x,
			.y = viewport.y,
			.width = viewport.width,
			.height = viewport.height,
			.minDepth = viewport.minDepth,
			.maxDepth = viewport.maxDepth};
		vk::CommandBuffer::SetViewport(vkViewport);
	}

	void VkRHICommandBuffer::SetScissor(const Rect2D& scissor)
	{
		const VkRect2D vkScissor = {
			.offset = {
				.x = scissor.x,
				.y = scissor.y},
			.extent = {.width = scissor.width, .height = scissor.height}};
		vk::CommandBuffer::SetScissor(vkScissor);
	}

	void VkRHICommandBuffer::BeginRenderPass(const rhi::RenderPass& renderPass, const rhi::FrameBuffer& frameBuffer, const Vector3f& clearColor)
	{
		CCT_AUTO_PROFILER_SCOPE();

		const VkRHIRenderPass& vkRenderPass = Cast<const VkRHIRenderPass&>(renderPass);
		const VkRHIFrameBuffer& vkRhiFrameBuffer = Cast<const VkRHIFrameBuffer&>(frameBuffer);

		const VkExtent2D extent = {
			.width = frameBuffer.GetWidth(),
			.height = frameBuffer.GetHeight()};

		const std::array clearValues = {
			VkClearValue{
				.color = {
					.float32 = {clearColor.X(), clearColor.Y(), clearColor.Z(), 0.f}}},
			VkClearValue{.depthStencil = {1.f, 0}}};
		const auto& attachments = vkRenderPass.GetAttachments();
		const bool anyClear = std::any_of(attachments.begin(), attachments.end(),
										  [](const VkAttachmentDescription& attachment)
										  {
											  return attachment.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ||
													 attachment.stencilLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR;
										  });

		VkRenderPassBeginInfo renderPassInfo = VulkanInitializer::RenderPassBeginInfo(*vkRenderPass.Get(), extent, *vkRhiFrameBuffer.Get());
		renderPassInfo.clearValueCount = anyClear ? static_cast<UInt32>(std::min(attachments.size(), clearValues.size())) : 0;
		renderPassInfo.pClearValues = renderPassInfo.clearValueCount != 0 ? clearValues.data() : nullptr;

		vk::CommandBuffer::BeginRenderPass(renderPassInfo);
	}

	void VkRHICommandBuffer::EndRenderPass()
	{
		CCT_AUTO_PROFILER_SCOPE();

		vk::CommandBuffer::EndRenderPass();
	}

	void VkRHICommandBuffer::BindMaterial(const Material& material)
	{
		CCT_AUTO_PROFILER_SCOPE();

		CCT_ASSERT(material.pipeline, "Invalid pointer");
		const auto& pipeline = Cast<const VkRHIPipeline&>(*material.pipeline);

		const auto& pipelineLayout = pipeline.GetLayout();

		vk::CommandBuffer::BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipeline());

		std::vector<VkDescriptorSet> descriptorSets;
		descriptorSets.resize(material.descriptorSets.size());
		for (std::size_t i = 0; i < material.descriptorSets.size(); ++i)
		{
			CCT_ASSERT(material.descriptorSets[i], "Invalid pointer");
			const auto& vkDescriptorSet = Cast<const VkRHIDescriptorSet&>(*material.descriptorSets[i]);
			CCT_ASSERT(vkDescriptorSet.Get(), "Invalid pointer");
			descriptorSets[i] = *vkDescriptorSet.Get()->Get();
		}

		vk::CommandBuffer::BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout.Get(), descriptorSets);
	}

	void VkRHICommandBuffer::BindVertexBuffer(const rhi::Buffer& buffer)
	{
		CCT_AUTO_PROFILER_SCOPE();

		const VkRHIBuffer& vkBuffer = Cast<const VkRHIBuffer&>(buffer);
		vk::CommandBuffer::BindVertexBuffers(vkBuffer);
	}

	void VkRHICommandBuffer::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		CCT_AUTO_PROFILER_SCOPE();

		vk::CommandBuffer::Draw(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void VkRHICommandBuffer::Copy(const Buffer& src, const Texture& dst)
	{
		const auto& vkBuffer = Cast<const VkRHIBuffer&>(src);
		const auto& vkTexture = Cast<const VkRHITexture&>(dst);

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
			.depth = 1};

		m_device->vkCmdCopyBufferToImage(
			*vk::CommandBuffer::Get(), *static_cast<const vk::Buffer&>(vkBuffer).Get(),
			*vkTexture.GetImage().Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &copyRegion);
	}

	void VkRHICommandBuffer::Copy(const Texture& src, const Buffer& dst, UInt64 dstOffset)
	{
		const auto& vkTexture = Cast<const VkRHITexture&>(src);
		const auto& vkBuffer = Cast<const VkRHIBuffer&>(dst);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = dstOffset;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;
		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = {
			.width = vkTexture.GetImage().GetExtent().width,
			.height = vkTexture.GetImage().GetExtent().height,
			.depth = 1};

		m_device->vkCmdCopyImageToBuffer(
			*vk::CommandBuffer::Get(), *vkTexture.GetImage().Get(),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			*static_cast<const vk::Buffer&>(vkBuffer).Get(),
			1, &copyRegion);
	}

	void VkRHICommandBuffer::TransitionImageLayout(const Texture& texture, ImageLayout oldLayout, ImageLayout newLayout)
	{
		const auto& vkTexture = Cast<const VkRHITexture&>(texture);

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = Converters::ToVulkan(oldLayout);
		barrier.newLayout = Converters::ToVulkan(newLayout);
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = *vkTexture.GetImage().Get();
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		if (oldLayout == ImageLayout::Undefined && newLayout == ImageLayout::TransferDstOptimal)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == ImageLayout::TransferDstOptimal && newLayout == ImageLayout::ShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == ImageLayout::ShaderReadOnlyOptimal && newLayout == ImageLayout::TransferDstOptimal)
		{
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == ImageLayout::TransferDstOptimal && newLayout == ImageLayout::ColorAttachmentOptimal)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == ImageLayout::Undefined && newLayout == ImageLayout::ColorAttachmentOptimal)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == ImageLayout::ColorAttachmentOptimal && newLayout == ImageLayout::ShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == ImageLayout::ShaderReadOnlyOptimal && newLayout == ImageLayout::ShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}

		m_device->vkCmdPipelineBarrier(
			*vk::CommandBuffer::Get(), srcStage, dstStage, 0,
			0, nullptr, 0, nullptr, 1, &barrier);
	}

	void VkRHICommandBuffer::BindPipeline(const Pipeline& pipeline)
	{
		const auto& vkPipeline = Cast<const VkRHIPipeline&>(pipeline);
		vk::CommandBuffer::BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline.GetPipeline());
	}

	void VkRHICommandBuffer::BindComputePipeline(const Pipeline& pipeline)
	{
		const auto& vkPipeline = Cast<const VkRHIPipeline&>(pipeline);
		vk::CommandBuffer::BindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE, vkPipeline.GetPipeline());
	}

	void VkRHICommandBuffer::BindComputeDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set)
	{
		const auto& vkLayout = Cast<const VkRHIPipelineLayout&>(layout);
		const auto& vkSet = Cast<const VkRHIDescriptorSet&>(set);
		vk::CommandBuffer::BindDescriptorSets(VK_PIPELINE_BIND_POINT_COMPUTE, *vkLayout.Get(), 0, 1, *vkSet.Get());
	}

	void VkRHICommandBuffer::Dispatch(UInt32 groupCountX, UInt32 groupCountY, UInt32 groupCountZ)
	{
		m_device->vkCmdDispatch(*vk::CommandBuffer::Get(), groupCountX, groupCountY, groupCountZ);
	}

	void VkRHICommandBuffer::BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set, UInt32 dynamicOffset)
	{
		const auto& vkLayout = Cast<const VkRHIPipelineLayout&>(layout);
		const auto& vkSet = Cast<const VkRHIDescriptorSet&>(set);
		vk::CommandBuffer::BindDescriptorSets(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			*vkLayout.Get(),
			0, 1, *vkSet.Get(), dynamicOffset);
	}

	void VkRHICommandBuffer::BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set)
	{
		const auto& vkLayout = Cast<const VkRHIPipelineLayout&>(layout);
		const auto& vkSet = Cast<const VkRHIDescriptorSet&>(set);
		vk::CommandBuffer::BindDescriptorSets(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			*vkLayout.Get(),
			0, 1, *vkSet.Get());
	}

	void VkRHICommandBuffer::BindIndexBuffer(const Buffer& buffer, bool use32bitIndices)
	{
		const auto& vkBuffer = Cast<const VkRHIBuffer&>(buffer);
		m_device->vkCmdBindIndexBuffer(
			*vk::CommandBuffer::Get(),
			*static_cast<const vk::Buffer&>(vkBuffer).Get(), 0,
			use32bitIndices ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
	}

	void VkRHICommandBuffer::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, Int32 vertexOffset, UInt32 firstInstance)
	{
		m_device->vkCmdDrawIndexed(*vk::CommandBuffer::Get(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void VkRHICommandBuffer::ClearTexture(const Texture& texture, const Vector4f& clearColor)
	{
		const auto& vkTexture = Cast<const VkRHITexture&>(texture);
		VkClearColorValue vkClearColor{{clearColor.X(), clearColor.Y(), clearColor.Z(), clearColor[3]}};
		VkImageSubresourceRange range{};
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.levelCount = 1;
		range.layerCount = 1;
		m_device->vkCmdClearColorImage(
			*vk::CommandBuffer::Get(),
			*vkTexture.GetImage().Get(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			&vkClearColor, 1, &range);
	}

	void VkRHICommandBuffer::ExecuteCommands(std::span<rhi::CommandBuffer*> secondaryCmdBufs)
	{
		std::vector<VkCommandBuffer> vkCmdBufs;
		vkCmdBufs.reserve(secondaryCmdBufs.size());
		for (auto* cmdBuf : secondaryCmdBufs)
		{
			auto& vkCmdBuf = Cast<VkRHICommandBuffer&>(*cmdBuf);
			vkCmdBufs.push_back(*static_cast<vk::CommandBuffer&>(vkCmdBuf).Get());
		}
		m_device->vkCmdExecuteCommands(*vk::CommandBuffer::Get(),
									   static_cast<UInt32>(vkCmdBufs.size()), vkCmdBufs.data());
	}
	void VkRHICommandBuffer::PipelineBarrier(const Texture& texture,
											 ImageLayout oldLayout,
											 ImageLayout newLayout,
											 PipelineStageFlags srcStage,
											 PipelineStageFlags dstStage,
											 MemoryAccessFlags srcAccess,
											 MemoryAccessFlags dstAccess)
	{
		const auto& vkTexture = Cast<const VkRHITexture&>(texture);

		const bool isDepthLayout =
			newLayout == ImageLayout::DepthStencilAttachmentOptimal ||
			newLayout == ImageLayout::DepthStencilReadOnlyOptimal ||
			newLayout == ImageLayout::DepthAttachmentOptimal ||
			newLayout == ImageLayout::DepthReadOnlyOptimal ||
			oldLayout == ImageLayout::DepthStencilAttachmentOptimal ||
			oldLayout == ImageLayout::DepthStencilReadOnlyOptimal ||
			oldLayout == ImageLayout::DepthAttachmentOptimal ||
			oldLayout == ImageLayout::DepthReadOnlyOptimal;

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = Converters::ToVulkan(oldLayout);
		barrier.newLayout = Converters::ToVulkan(newLayout);
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = *vkTexture.GetImage().Get();
		barrier.subresourceRange.aspectMask = isDepthLayout ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = Converters::ToVulkan(srcAccess);
		barrier.dstAccessMask = Converters::ToVulkan(dstAccess);

		m_device->vkCmdPipelineBarrier(
			*vk::CommandBuffer::Get(),
			Converters::ToVulkan(srcStage),
			Converters::ToVulkan(dstStage),
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	void VkRHICommandBuffer::PipelineBarrier(const Buffer& buffer,
											 PipelineStageFlags srcStage,
											 PipelineStageFlags dstStage,
											 MemoryAccessFlags srcAccess,
											 MemoryAccessFlags dstAccess)
	{
		const auto& vkBuffer = Cast<const VkRHIBuffer&>(buffer);

		VkBufferMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = *static_cast<const vk::Buffer&>(vkBuffer).Get();
		barrier.offset = 0;
		barrier.size = VK_WHOLE_SIZE;
		barrier.srcAccessMask = Converters::ToVulkan(srcAccess);
		barrier.dstAccessMask = Converters::ToVulkan(dstAccess);

		m_device->vkCmdPipelineBarrier(
			*vk::CommandBuffer::Get(),
			Converters::ToVulkan(srcStage),
			Converters::ToVulkan(dstStage),
			0,
			0, nullptr,
			1, &barrier,
			0, nullptr);
	}

	void VkRHICommandBuffer::BeginDebugLabel(const char* name, float r, float g, float b)
	{
		if (m_device->vkCmdDebugMarkerBeginEXT == nullptr || !m_device->IsExtensionEnabled(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
			return;
		VkDebugMarkerMarkerInfoEXT info{};
		info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
		info.pMarkerName = name;
		info.color[0] = r;
		info.color[1] = g;
		info.color[2] = b;
		info.color[3] = 1.F;
		m_device->vkCmdDebugMarkerBeginEXT(*vk::CommandBuffer::Get(), &info);
	}

	void VkRHICommandBuffer::EndDebugLabel()
	{
		if (m_device->vkCmdDebugMarkerEndEXT == nullptr || !m_device->IsExtensionEnabled(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
			return;
		m_device->vkCmdDebugMarkerEndEXT(*vk::CommandBuffer::Get());
	}

	void* VkRHICommandBuffer::GetNativeHandle() const
	{
		return static_cast<void*>(*vk::CommandBuffer::Get());
	}
} // namespace cct::gfx::rhi
