//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHICOMMANDBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHICOMMANDBUFFER_HPP

#include <span>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandList/CommandList.hpp"
#include "Concerto/Graphics/RHI/CommandBuffer.hpp"

namespace cct::gfx::dx12
{
	class Dx12RootSignature;
}

namespace cct::gfx::rhi
{
	class Dx12RHIFrameBuffer;
	class Dx12RHIDevice;
	class Dx12RHIPipeline;
	class Dx12RHIDescriptorSet;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHICommandBuffer : public rhi::CommandBuffer, public dx12::CommandList
	{
	public:
		Dx12RHICommandBuffer(dx12::CommandAllocator& owner, D3D12_COMMAND_LIST_TYPE type, Dx12RHIDevice& device);
		Dx12RHICommandBuffer() = default;

		void Begin() override;
		void End() override;
		void Reset() override;
		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Rect2D& scissor) override;
		void BeginRenderPass(const rhi::RenderPass& renderPass, const rhi::FrameBuffer& frameBuffer, const Vector3f& clearColor) override;
		void EndRenderPass() override;
		void BindMaterial(const Material& material) override;
		void BindPipeline(const Pipeline& pipeline) override;
		void BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set) override;
		void BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set, UInt32 dynamicOffset) override;
		void BindIndexBuffer(const Buffer& buffer, bool use32bitIndices) override;
		void DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, Int32 vertexOffset,
						 UInt32 firstInstance) override;
		void ClearTexture(const Texture& texture, const Vector4f& clearColor) override;
		void BindVertexBuffer(const rhi::Buffer& buffer) override;
		void Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance) override;
		void Copy(const Buffer& src, const Texture& dst) override;
		void Copy(const Texture& src, const Buffer& dst, UInt64 dstOffset = 0) override;
		void TransitionImageLayout(const Texture& texture, ImageLayout oldLayout, ImageLayout newLayout) override;
		void ExecuteCommands(std::span<CommandBuffer*> secondaryCmdBufs) override;
		void PipelineBarrier(const Texture& texture,
							 ImageLayout oldLayout,
							 ImageLayout newLayout,
							 PipelineStageFlags srcStage,
							 PipelineStageFlags dstStage,
							 MemoryAccessFlags srcAccess,
							 MemoryAccessFlags dstAccess) override;
		void PipelineBarrier(const Buffer& buffer,
							 PipelineStageFlags srcStage,
							 PipelineStageFlags dstStage,
							 MemoryAccessFlags srcAccess,
							 MemoryAccessFlags dstAccess) override;
		void PipelineBarrier(std::span<const TextureBarrier> barriers) override;
		void BindComputePipeline(const Pipeline& pipeline) override;
		void BindComputeDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set) override;
		void Dispatch(UInt32 groupCountX, UInt32 groupCountY, UInt32 groupCountZ) override;
		void BeginDebugLabel(const char* name, float r, float g, float b) override;
		void EndDebugLabel() override;
		void* GetNativeHandle() const override;

	private:
		static D3D12_RESOURCE_STATES ToD3D12ResourceState(ImageLayout layout);
		void SetHeapsAndRootSignature(const Dx12RHIPipeline& pipeline, bool isCompute);
		void BindDescriptorSetImpl(const dx12::Dx12RootSignature& rootSig, const Dx12RHIDescriptorSet& set, UINT setIndex, bool isCompute);
		Dx12RHIDevice* m_device = nullptr;
		const Dx12RHIFrameBuffer* m_currentFrameBuffer = nullptr;
		UInt32 m_currentVertexStride = 0;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHICOMMANDBUFFER_HPP
