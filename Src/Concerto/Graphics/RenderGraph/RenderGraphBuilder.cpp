//
// Created by arthur on 11/05/2026.
//

#include "Concerto/Graphics/RenderGraph/RenderGraphBuilder.hpp"

#include "Concerto/Graphics/RenderGraph/RenderGraphPass.hpp"

namespace cct::gfx::rhi
{
	RenderGraphBuilder::RenderGraphBuilder(RGPass& pass) :
		m_pass(pass)
	{
	}

	RGTextureHandle RenderGraphBuilder::Read(RGTextureHandle handle)
	{
		m_pass.textureUsages.push_back({handle, RGResourceAccess::Read, false});
		return handle;
	}

	RGTextureHandle RenderGraphBuilder::Write(RGTextureHandle handle)
	{
		m_pass.textureUsages.push_back({handle, RGResourceAccess::Write, false});
		return handle;
	}

	RGTextureHandle RenderGraphBuilder::WriteDepth(RGTextureHandle handle)
	{
		m_pass.textureUsages.push_back({handle, RGResourceAccess::Write, true});
		return handle;
	}

	RGBufferHandle RenderGraphBuilder::Read(RGBufferHandle handle)
	{
		m_pass.bufferUsages.push_back({handle, RGResourceAccess::Read});
		return handle;
	}

	RGBufferHandle RenderGraphBuilder::Write(RGBufferHandle handle)
	{
		m_pass.bufferUsages.push_back({handle, RGResourceAccess::Write});
		return handle;
	}

	void RenderGraphBuilder::SetSideEffect()
	{
		m_pass.sideEffects = true;
	}
} // namespace cct::gfx::rhi
