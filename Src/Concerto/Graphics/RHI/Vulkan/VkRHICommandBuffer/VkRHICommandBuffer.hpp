//
// Created by arthur on 03/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_VKRHICOMMANDBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_VKRHICOMMANDBUFFER_HPP

#include "Concerto/Graphics/RHI/CommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHICommandBuffer final: public rhi::CommandBuffer, public vk::CommandBuffer
	{
	public:
		VkRHICommandBuffer(VkRHIDevice& device, vk::CommandPool& commandPool, CommandBufferUsage usage);

		void Begin() override;
		void End() override;
		void Reset() override;
		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Rect2D& scissor) override;
		void BeginRenderPass(const rhi::RenderPass& renderPass, const rhi::FrameBuffer& frameBuffer, const Vector3f& clearColor) override;
		void EndRenderPass() override;
		void BindMaterial(const Material& material) override;
		void BindVertexBuffer(const rhi::Buffer& buffer) override;
		void Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance) override;
		void Copy(const Buffer& src, const Texture& dst) override;
		void Copy(const Texture& src, const Buffer& dst) override;
		void TransitionImageLayout(const Texture& texture, ImageLayout oldLayout, ImageLayout newLayout) override;

		void BindPipeline(const Pipeline& pipeline) override;
		void BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set, UInt32 dynamicOffset) override;
		void BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set) override;
		void BindIndexBuffer(const Buffer& buffer, bool use32bitIndices) override;
		void DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, Int32 vertexOffset, UInt32 firstInstance) override;
		void ClearTexture(const Texture& texture, const Vector4f& clearColor) override;
		void ExecuteCommands(std::span<CommandBuffer*> secondaryCmdBufs) override;

	private:
		VkRHIDevice* m_device = nullptr;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VKRHICOMMANDBUFFER_HPP