#include "Concerto/Graphics/Renderer/Renderer.hpp"

namespace cct::gfx
{
	Renderer::Renderer(rhi::Device& device, UInt32 maxFramesInFlight) :
		m_graph(device, maxFramesInFlight)
	{
	}

	void Renderer::AddFeature(std::unique_ptr<RenderFeature> feature)
	{
		m_features.push_back(std::move(feature));
	}

	void Renderer::Build(const RenderTarget& target)
	{
		Rebuild(target);
	}

	const rhi::RenderPass& Renderer::GetPassRenderPass(const char* name) const
	{
		return m_graph.GetPassRenderPass(name);
	}

	void Renderer::Rebuild(const RenderTarget& target)
	{
		m_graph.Clear();

		m_resources.backbuffer = m_graph.ImportTexture(
			"Backbuffer", target.colorTexture, target.currentLayout, target.colorFormat, target.width, target.height);
		m_resources.depth = m_graph.CreateTexture(
			{target.width, target.height, target.depthFormat, true, "SceneDepth"});

		for (const auto& feature : m_features)
			feature->Setup(m_graph, m_resources);

		m_graph.SetFinalOutput(m_resources.backbuffer);
		m_graph.SetExportLayout(m_resources.backbuffer, target.exportLayout);

		m_declaredWidth = target.width;
		m_declaredHeight = target.height;

		m_graph.Compile();
	}

	void Renderer::DrawFrame(rhi::CommandBuffer& cmd, const RenderTarget& target, const View& view)
	{
		m_graph.Reset();

		if (target.width != m_declaredWidth || target.height != m_declaredHeight)
			Rebuild(target);
		else
			m_graph.UpdateImportedTexture(m_resources.backbuffer, target.colorTexture, target.currentLayout);

		for (const auto& feature : m_features)
			feature->UpdateFrameData(view);

		m_graph.Execute(cmd, target.width, target.height);
	}
} // namespace cct::gfx
