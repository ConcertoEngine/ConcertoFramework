//
// Created by arthur on 18/02/2023.
//

#ifndef CONCERTO_GRAPHICS_RHI_GPUMESH_HPP
#define CONCERTO_GRAPHICS_RHI_GPUMESH_HPP

#include <memory>
#include <vector>

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class GpuSubMesh;
	class CONCERTO_GRAPHICS_RHI_BASE_API GpuMesh
	{
	public:
		virtual ~GpuMesh() = default;
		std::vector<std::shared_ptr<GpuSubMesh>> subMeshes;
	};

	using GpuMeshPtr = std::shared_ptr<GpuMesh>;
} // namespace cct::gfx::rhi
#endif // CONCERTO_GRAPHICS_RHI_GPUMESH_HPP
