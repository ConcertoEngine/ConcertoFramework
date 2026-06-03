//
// Created by arthur on 03/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_COMMANDBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_COMMANDBUFFER_HPP

#include <span>

#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Utils/Utils.hpp"

namespace cct::gfx::rhi
{
	class RenderPass;
	class Buffer;
	class Texture;
	class FrameBuffer;
	class Material;
	class Pipeline;
	class PipelineLayout;
	class DescriptorSet;

	class CONCERTO_GRAPHICS_RHI_BASE_API CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Reset() = 0;
		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetScissor(const Rect2D& scissor) = 0;
		virtual void BeginRenderPass(const rhi::RenderPass& renderPass, const rhi::FrameBuffer& frameBuffer, const Vector3f& clearColor) = 0;
		virtual void EndRenderPass() = 0;
		virtual void BindMaterial(const Material& material) = 0;
		virtual void BindVertexBuffer(const rhi::Buffer& buffer) = 0;
		virtual void Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance) = 0;
		virtual void Copy(const Buffer& src, const Texture& dst) = 0;
		virtual void Copy(const Texture& src, const Buffer& dst) = 0;
		virtual void TransitionImageLayout(const Texture& texture, ImageLayout oldLayout, ImageLayout newLayout) = 0;

		virtual void BindPipeline(const Pipeline& pipeline)
		{
		}
		virtual void BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set, UInt32 dynamicOffset)
		{
		}
		virtual void BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set)
		{
		}
		virtual void BindIndexBuffer(const Buffer& buffer, bool use32bitIndices)
		{
		}
		virtual void DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, Int32 vertexOffset, UInt32 firstInstance)
		{
		}
		virtual void ClearTexture(const Texture& texture, const Vector4f& clearColor)
		{
		}
		virtual void ExecuteCommands(std::span<CommandBuffer*> /*secondaryCmdBufs*/)
		{
		}

		virtual void PipelineBarrier(const Texture& texture,
									 ImageLayout oldLayout,
									 ImageLayout newLayout,
									 PipelineStageFlags srcStage,
									 PipelineStageFlags dstStage,
									 MemoryAccessFlags srcAccess,
									 MemoryAccessFlags dstAccess)
		{
		}

		virtual void BeginDebugLabel(const char* name, float r = 1.F, float g = 1.F, float b = 1.F)
		{
		}
		virtual void EndDebugLabel()
		{
		}
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_COMMANDBUFFER_HPP