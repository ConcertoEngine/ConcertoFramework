#ifndef CONCERTO_GRAPHICS_RENDERER_FORWARDOPAQUEFEATURE_HPP
#define CONCERTO_GRAPHICS_RENDERER_FORWARDOPAQUEFEATURE_HPP

#include "Concerto/Graphics/Renderer/RenderFeature.hpp"
#include "Concerto/Graphics/RHI/GpuMesh.hpp"

namespace cct::gfx
{
	class CONCERTO_GRAPHICS_RHI_BASE_API ForwardOpaqueFeature final : public RenderFeature
	{
	public:
		void SetMesh(rhi::GpuMeshPtr mesh);

		void Setup(rhi::RenderGraph& graph, FrameResources& resources) override;

	private:
		rhi::GpuMeshPtr m_mesh;
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_RENDERER_FORWARDOPAQUEFEATURE_HPP
