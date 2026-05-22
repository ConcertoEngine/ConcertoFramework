//
// Created by arthur on 11/05/2026.
//

#include "Concerto/Graphics/RenderGraph/RenderGraphContext.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/RenderGraph/RenderGraphResourceRegistry.hpp"

namespace cct::gfx::rhi
{
	RenderGraphContext::RenderGraphContext(CommandBuffer& cmd,
										   FrameBuffer* frameBuffer,
										   const RenderPass* renderPass,
										   RenderGraphResourceRegistry& registry,
										   UInt32 width, UInt32 height) :
		m_cmd(cmd),
		m_frameBuffer(frameBuffer),
		m_renderPass(renderPass),
		m_registry(registry),
		m_width(width),
		m_height(height)
	{
	}

	CommandBuffer& RenderGraphContext::GetCommandBuffer()
	{
		return m_cmd;
	}

	Texture& RenderGraphContext::GetTexture(RGTextureHandle handle)
	{
		return m_registry.GetTexture(handle);
	}

	Buffer& RenderGraphContext::GetBuffer(RGBufferHandle handle)
	{
		return m_registry.GetBuffer(handle);
	}

	FrameBuffer& RenderGraphContext::GetFrameBuffer()
	{
		CCT_ASSERT(m_frameBuffer != nullptr, "RenderGraphContext::GetFrameBuffer: not a Graphics pass");
		return *m_frameBuffer;
	}

	const RenderPass& RenderGraphContext::GetRenderPass()
	{
		CCT_ASSERT(m_renderPass != nullptr, "RenderGraphContext::GetRenderPass: not a Graphics pass");
		return *m_renderPass;
	}
} // namespace cct::gfx::rhi
