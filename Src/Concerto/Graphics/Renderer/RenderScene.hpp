#ifndef CONCERTO_GRAPHICS_RENDERER_RENDERSCENE_HPP
#define CONCERTO_GRAPHICS_RENDERER_RENDERSCENE_HPP

#include <vector>

#include "Concerto/Graphics/Renderer/MeshInstance.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx
{
	class CONCERTO_GRAPHICS_RHI_BASE_API RenderScene
	{
	public:
		void AddInstance(MeshInstance instance);

		[[nodiscard]] const std::vector<MeshInstance>& GetInstances() const;

	private:
		std::vector<MeshInstance> m_instances;
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_RENDERER_RENDERSCENE_HPP
