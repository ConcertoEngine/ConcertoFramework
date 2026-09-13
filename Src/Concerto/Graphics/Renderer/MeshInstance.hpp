#ifndef CONCERTO_GRAPHICS_RENDERER_MESHINSTANCE_HPP
#define CONCERTO_GRAPHICS_RENDERER_MESHINSTANCE_HPP

#include <Concerto/Core/Math/Matrix/Matrix.hpp>

#include "Concerto/Graphics/RHI/GpuMesh.hpp"

namespace cct::gfx
{
	struct MeshInstance
	{
		rhi::GpuMeshPtr mesh;
		Matrix4f transform;
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_RENDERER_MESHINSTANCE_HPP
