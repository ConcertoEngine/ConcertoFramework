#include "Concerto/Graphics/Renderer/RenderScene.hpp"

namespace cct::gfx
{
	void RenderScene::AddInstance(MeshInstance instance)
	{
		m_instances.push_back(std::move(instance));
	}

	const std::vector<MeshInstance>& RenderScene::GetInstances() const
	{
		return m_instances;
	}
} // namespace cct::gfx
