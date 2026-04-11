//
// Created by arthur on 18/02/2023.
//

#ifndef CONCERTO_GRAPHICS_RHI_GPUMESH_HPP
#define CONCERTO_GRAPHICS_RHI_GPUMESH_HPP

#include <memory>
#include <vector>

namespace cct::gfx::rhi
{
	class GpuSubMesh;
	class CONCERTO_GRAPHICS_RHI_BASE_API GpuMesh
	{
	public:
		virtual ~GpuMesh() = default;
		Matrix4f transformMatrix;
		std::vector<std::shared_ptr<GpuSubMesh>> subMeshes;
	};

	using GpuMeshPtr = std::shared_ptr<GpuMesh>;
}
#endif //CONCERTO_GRAPHICS_RHI_GPUMESH_HPP
