//
// Created by arthur on 03/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_VKRHICOMMANDBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_VKRHICOMMANDBUFFER_HPP

#include <span>

#include "Concerto/Graphics/RHI/CommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"

namespace cct::gfx::vk
{
	class DescriptorSet;
}

namespace cct::gfx::rhi
{
	class VkRHIPipeline;
	class VkRHIPipelineLayout;
	class VkRHIDescriptorSet;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHICommandBuffer final: public rhi::CommandBuffer, public vk::CommandBuffer
	{
	public:
		VkRHICommandBuffer(VkRHIDevice& device, vk::CommandPool& commandPool);

		// ── rhi::CommandBuffer interface ──────────────────────────────────────
		void Begin() override;
		void End() override;
		void Submit() override;
		void Reset() override;
		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Rect2D& scissor) override;
		void BeginRenderPass(const rhi::RenderPass& renderPass, const rhi::FrameBuffer& frameBuffer, const Vector3f& clearColor) override;
		void EndRenderPass() override;
		void BindMaterial(const MaterialInfo& material) override;
		void BindVertexBuffer(const rhi::Buffer& buffer) override;
		void Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance) override;
		void BindIndexBuffer(const rhi::Buffer& buffer, bool use32bitIndices = true) override;
		void DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, Int32 vertexOffset, UInt32 firstInstance) override;
		void TransitionImageLayout(rhi::Texture& texture, rhi::ImageLayout oldLayout, rhi::ImageLayout newLayout,
		                           rhi::PipelineStageFlags srcStage, rhi::PipelineStageFlags dstStage,
		                           rhi::MemoryAccessFlags srcAccess, rhi::MemoryAccessFlags dstAccess) override;
		void ClearColorImage(rhi::Texture& texture, rhi::ImageLayout layout, float r, float g, float b, float a) override;

		// ── Vulkan-only (not on rhi::CommandBuffer) ───────────────────────────
		void BindPipeline(VkPipelineBindPoint bindPoint, const VkRHIPipeline& pipeline);
		void BindDescriptorSet(VkPipelineBindPoint bindPoint, const VkRHIPipelineLayout& layout,
		                       const VkRHIDescriptorSet& set, UInt32 dynamicOffset = ~0U);
		void UpdateDescriptorSets(std::span<VkWriteDescriptorSet> writes);

	private:
		VkRHIDevice& m_device;
		VkPipeline m_lastBoundedPipeline = {};
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VKRHICOMMANDBUFFER_HPP
