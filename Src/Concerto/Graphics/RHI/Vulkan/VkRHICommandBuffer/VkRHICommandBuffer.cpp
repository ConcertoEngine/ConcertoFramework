//
// Created by arthur on 03/09/2024.
//

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Pipeline/Pipeline.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"

#include "Concerto/Graphics/RHI/Vulkan/VkMaterial.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandBuffer/VkRHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIBuffer/VkRHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIRenderPass/VkRHIRenderPass.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHISwapChain/VkRHISwapChain.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIFrameBuffer/VKRHIFrameBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipeline/VkRHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipelineLayout/VkRHIPipelineLayout.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSet/VkRHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"

namespace cct::gfx::rhi
{
	VkRHICommandBuffer::VkRHICommandBuffer(VkRHIDevice& device, vk::CommandPool& commandPool) :
		vk::CommandBuffer(commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY)),
		m_device(device)
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

	void VkRHICommandBuffer::Submit()
	{
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
			.maxDepth = viewport.maxDepth
		};
		vk::CommandBuffer::SetViewport(vkViewport);
	}

	void VkRHICommandBuffer::SetScissor(const Rect2D& scissor)
	{
		const VkRect2D vkScissor = {
			.offset = {
				.x = scissor.x,
				.y = scissor.x
			},
			.extent = {
				.width = scissor.width,
				.height = scissor.height
			}
		};
		vk::CommandBuffer::SetScissor(vkScissor);
	}

	void VkRHICommandBuffer::BeginRenderPass(const rhi::RenderPass& renderPass, const rhi::FrameBuffer& frameBuffer, const Vector3f& clearColor)
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();

		const VkRHIRenderPass& vkRenderPass = Cast<const VkRHIRenderPass&>(renderPass);
		const VkRHIFrameBuffer& vkRhiFrameBuffer = Cast<const VkRHIFrameBuffer&>(frameBuffer);

		const VkExtent2D extent = {
			.width = frameBuffer.GetWidth(),
			.height = frameBuffer.GetHeight()
		};

		const std::array clearValues = {
					VkClearValue  {
					.color = {
						.float32 = {	clearColor.X(), clearColor.Y(), clearColor.Z(), 0.f}
					}
				},
			VkClearValue {
				.depthStencil = {1.f, 0}
			}
		};
		VkRenderPassBeginInfo renderPassInfo = VulkanInitializer::RenderPassBeginInfo(*vkRenderPass.Get(), extent, *vkRhiFrameBuffer.Get());
		renderPassInfo.clearValueCount = static_cast<UInt32>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vk::CommandBuffer::BeginRenderPass(renderPassInfo);
	}

	void VkRHICommandBuffer::EndRenderPass()
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();

		vk::CommandBuffer::EndRenderPass();
	}

	void VkRHICommandBuffer::BindMaterial(const MaterialInfo& material)
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();

		const vk::VkMaterial& vkMaterial = Cast<const vk::VkMaterial&>(material);
		const VkPipeline pipeline = *vkMaterial.pipeline->Get();
		//if (m_lastBoundedPipeline == pipeline)
		//	return;
		vk::CommandBuffer::BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		//_lastBoundedPipeline = pipeline;
		auto cpy = vkMaterial.descriptorSets; //fixme
		vk::CommandBuffer::BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, *vkMaterial.pipeline->GetPipelineLayout()->Get(), cpy);
	}

	void VkRHICommandBuffer::BindVertexBuffer(const rhi::Buffer& buffer)
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();

		const VkRHIBuffer& vkBuffer = Cast<const VkRHIBuffer&>(buffer);
		vk::CommandBuffer::BindVertexBuffers(vkBuffer);
	}

	void VkRHICommandBuffer::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();

		vk::CommandBuffer::Draw(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void VkRHICommandBuffer::BindIndexBuffer(const rhi::Buffer& buffer, bool use32bitIndices)
	{
		const VkRHIBuffer& vkBuffer = Cast<const VkRHIBuffer&>(buffer);
		m_device.vkCmdBindIndexBuffer(*Get(), *vkBuffer.Get(), 0,
			use32bitIndices ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
	}

	void VkRHICommandBuffer::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, Int32 vertexOffset, UInt32 firstInstance)
	{
		m_device.vkCmdDrawIndexed(*Get(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void VkRHICommandBuffer::TransitionImageLayout(rhi::Texture& texture, rhi::ImageLayout oldLayout, rhi::ImageLayout newLayout,
	                                               rhi::PipelineStageFlags srcStage, rhi::PipelineStageFlags dstStage,
	                                               rhi::MemoryAccessFlags srcAccess, rhi::MemoryAccessFlags dstAccess)
	{
		VkRHITexture& vkTexture = Cast<VkRHITexture&>(texture);

		VkImageMemoryBarrier barrier{};
		barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout           = Converters::ToVulkan(oldLayout);
		barrier.newLayout           = Converters::ToVulkan(newLayout);
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image               = *vkTexture.GetImage().Get();
		barrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		barrier.srcAccessMask       = Converters::ToVulkan<VkAccessFlagBits>(srcAccess);
		barrier.dstAccessMask       = Converters::ToVulkan<VkAccessFlagBits>(dstAccess);

		m_device.vkCmdPipelineBarrier(
			*Get(),
			Converters::ToVulkan<VkPipelineStageFlagBits>(srcStage),
			Converters::ToVulkan<VkPipelineStageFlagBits>(dstStage),
			0, 0, nullptr, 0, nullptr, 1, &barrier);
	}

	void VkRHICommandBuffer::ClearColorImage(rhi::Texture& texture, rhi::ImageLayout layout, float r, float g, float b, float a)
	{
		VkRHITexture& vkTexture = Cast<VkRHITexture&>(texture);

		VkClearColorValue clearColor{};
		clearColor.float32[0] = r;
		clearColor.float32[1] = g;
		clearColor.float32[2] = b;
		clearColor.float32[3] = a;

		VkImageSubresourceRange range{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		m_device.vkCmdClearColorImage(*Get(), *vkTexture.GetImage().Get(),
			Converters::ToVulkan(layout), &clearColor, 1, &range);
	}

	// ── Vulkan-only methods ───────────────────────────────────────────────────

	void VkRHICommandBuffer::BindPipeline(VkPipelineBindPoint bindPoint, const VkRHIPipeline& pipeline)
	{
		vk::CommandBuffer::BindPipeline(bindPoint, *pipeline.Get());
	}

	void VkRHICommandBuffer::BindDescriptorSet(VkPipelineBindPoint bindPoint, const VkRHIPipelineLayout& layout,
	                                           const VkRHIDescriptorSet& set, UInt32 dynamicOffset)
	{
		if (dynamicOffset == ~0U)
		{
			vk::CommandBuffer::BindDescriptorSets(bindPoint, *layout.Get(), 0, 1, set);
		}
		else
		{
			vk::CommandBuffer::BindDescriptorSets(bindPoint, *layout.Get(), 0, 1, set, dynamicOffset);
		}
	}

	void VkRHICommandBuffer::UpdateDescriptorSets(std::span<VkWriteDescriptorSet> writes)
	{
		m_device.UpdateDescriptorSetsWrite(writes);
	}
}
