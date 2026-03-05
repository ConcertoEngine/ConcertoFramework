//
// Created by arthur on 03/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_COMMANDBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_COMMANDBUFFER_HPP

#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/RHI/Utils/Utils.hpp"

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIMesh/VkRHIMesh.hpp"

namespace cct::gfx::rhi
{
	class RenderPass;
	class Buffer;
	class FrameBuffer;
	class Texture;

	class CONCERTO_GRAPHICS_RHI_BASE_API CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit() = 0;
		virtual void Reset() = 0;
		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetScissor(const Rect2D& scissor) = 0;
		virtual void BeginRenderPass(const rhi::RenderPass& renderPass, const rhi::FrameBuffer& frameBuffer, const Vector3f& clearColor) = 0;
		virtual void EndRenderPass() = 0;
		virtual void BindMaterial(const MaterialInfo& material) = 0;
		virtual void BindVertexBuffer(const rhi::Buffer& buffer) = 0;
		virtual void Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance) = 0;
		virtual void BindIndexBuffer(const rhi::Buffer& buffer, bool use32bitIndices = true) = 0;
		virtual void DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, Int32 vertexOffset, UInt32 firstInstance) = 0;
		virtual void TransitionImageLayout(rhi::Texture& texture, rhi::ImageLayout oldLayout, rhi::ImageLayout newLayout,
		                                   rhi::PipelineStageFlags srcStage, rhi::PipelineStageFlags dstStage,
		                                   rhi::MemoryAccessFlags srcAccess, rhi::MemoryAccessFlags dstAccess) = 0;
		virtual void ClearColorImage(rhi::Texture& texture, rhi::ImageLayout layout, float r, float g, float b, float a) = 0;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_COMMANDBUFFER_HPP