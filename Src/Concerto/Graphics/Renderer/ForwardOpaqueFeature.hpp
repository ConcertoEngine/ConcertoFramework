#ifndef CONCERTO_GRAPHICS_RENDERER_FORWARDOPAQUEFEATURE_HPP
#define CONCERTO_GRAPHICS_RENDERER_FORWARDOPAQUEFEATURE_HPP

#include <Concerto/Core/Math/Frustum/Frustum.hpp>
#include <Concerto/Core/Math/Matrix/Matrix.hpp>

#include "Concerto/Graphics/Renderer/RenderFeature.hpp"

namespace cct::gfx
{
	class RenderScene;

	class CONCERTO_GRAPHICS_RHI_BASE_API ForwardOpaqueFeature final : public RenderFeature
	{
	public:
		explicit ForwardOpaqueFeature(const RenderScene& scene);

		void Setup(rhi::RenderGraph& graph, FrameResources& resources) override;
		void UpdateFrameData(const View& view) override;

	private:
		const RenderScene& m_scene;
		Frustum m_frustum = Frustum::FromViewProjection(Matrix4f::Identity());
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_RENDERER_FORWARDOPAQUEFEATURE_HPP
