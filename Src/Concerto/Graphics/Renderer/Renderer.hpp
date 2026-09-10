#ifndef CONCERTO_GRAPHICS_RENDERER_RENDERER_HPP
#define CONCERTO_GRAPHICS_RENDERER_RENDERER_HPP

#include <memory>
#include <vector>

#include "Concerto/Graphics/Renderer/RenderFeature.hpp"
#include "Concerto/Graphics/Renderer/RenderTarget.hpp"
#include "Concerto/Graphics/Renderer/View.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraph.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"

namespace cct::gfx
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Renderer
	{
	public:
		explicit Renderer(rhi::Device& device, UInt32 maxFramesInFlight = 3);
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		void AddFeature(std::unique_ptr<RenderFeature> feature);

		void Build(const RenderTarget& target);

		void DrawFrame(rhi::CommandBuffer& cmd, const RenderTarget& target, const View& view);

		[[nodiscard]] const rhi::RenderPass& GetPassRenderPass(const char* name) const;

	private:
		void Rebuild(const RenderTarget& target);

		rhi::RenderGraph m_graph;
		std::vector<std::unique_ptr<RenderFeature>> m_features;
		FrameResources m_resources;
		UInt32 m_declaredWidth = 0;
		UInt32 m_declaredHeight = 0;
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_RENDERER_RENDERER_HPP
