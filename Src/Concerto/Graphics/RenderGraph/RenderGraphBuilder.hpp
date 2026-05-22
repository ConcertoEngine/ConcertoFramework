//
// Created by arthur on 11/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_RENDERGRAPH_BUILDER_HPP
#define CONCERTO_GRAPHICS_RHI_RENDERGRAPH_BUILDER_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphResource.hpp"

namespace cct::gfx::rhi
{
	struct RGPass;

	class CONCERTO_GRAPHICS_RHI_BASE_API RenderGraphBuilder
	{
	public:
		RGTextureHandle Read(RGTextureHandle handle);
		RGTextureHandle Write(RGTextureHandle handle);
		RGTextureHandle WriteDepth(RGTextureHandle handle);
		RGBufferHandle  Read(RGBufferHandle handle);
		RGBufferHandle  Write(RGBufferHandle handle);
		void SetSideEffect();

	private:
		friend class RenderGraph;
		explicit RenderGraphBuilder(RGPass& pass);
		RGPass& m_pass;
	};
}

#endif // CONCERTO_GRAPHICS_RHI_RENDERGRAPH_BUILDER_HPP
