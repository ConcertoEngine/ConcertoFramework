#ifndef CONCERTO_GRAPHICS_RENDERER_RENDERFEATURE_HPP
#define CONCERTO_GRAPHICS_RENDERER_RENDERFEATURE_HPP

#include "Concerto/Graphics/RenderGraph/RenderGraph.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphResource.hpp"
#include "Concerto/Graphics/Renderer/View.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx
{
	struct FrameResources
	{
		rhi::RGTextureHandle backbuffer;
		rhi::RGTextureHandle depth;
	};

	class CONCERTO_GRAPHICS_RHI_BASE_API RenderFeature
	{
	public:
		virtual ~RenderFeature() = default;
		virtual void Setup(rhi::RenderGraph& graph, FrameResources& resources) = 0;
		virtual void UpdateFrameData(const View& view)
		{
			(void)view;
		}
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_RENDERER_RENDERFEATURE_HPP
