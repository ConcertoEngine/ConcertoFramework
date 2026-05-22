//
// Created by arthur on 11/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_RENDERGRAPH_CONTEXT_HPP
#define CONCERTO_GRAPHICS_RHI_RENDERGRAPH_CONTEXT_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/CommandBuffer.hpp"
#include "Concerto/Graphics/RHI/FrameBuffer.hpp"
#include "Concerto/Graphics/RHI/RenderPass.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphResource.hpp"

namespace cct::gfx::rhi
{
	class RenderGraphResourceRegistry;

	class CONCERTO_GRAPHICS_RHI_BASE_API RenderGraphContext
	{
	public:
		CommandBuffer& GetCommandBuffer();
		Texture& GetTexture(RGTextureHandle handle);
		Buffer& GetBuffer(RGBufferHandle  handle);
		FrameBuffer& GetFrameBuffer();
		const RenderPass& GetRenderPass();

		[[nodiscard]] UInt32 GetWidth()  const { return m_width;  }
		[[nodiscard]] UInt32 GetHeight() const { return m_height; }

	private:
		friend class RenderGraph;
		RenderGraphContext(CommandBuffer& cmd,
		                   FrameBuffer* frameBuffer,
		                   const RenderPass* renderPass,
		                   RenderGraphResourceRegistry& registry,
		                   UInt32 width, UInt32 height);

		CommandBuffer& m_cmd;
		FrameBuffer* m_frameBuffer;
		const RenderPass* m_renderPass;
		RenderGraphResourceRegistry& m_registry;
		UInt32 m_width;
		UInt32 m_height;
	};
}

#endif // CONCERTO_GRAPHICS_RHI_RENDERGRAPH_CONTEXT_HPP
