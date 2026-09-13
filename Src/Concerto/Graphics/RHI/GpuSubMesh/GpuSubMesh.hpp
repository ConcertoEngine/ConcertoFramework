//
// Created by arthur on 09/06/22.
//

#ifndef CONCERTO_GRAPHICS_RHI_GPUSUBMESH_HPP
#define CONCERTO_GRAPHICS_RHI_GPUSUBMESH_HPP

#include <memory>

#include <Concerto/Core/Math/AABB/AABB.hpp>

#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Material/MaterialInstance.hpp"
#include "Concerto/Graphics/RHI/SubMesh/SubMesh.hpp"

namespace cct::gfx::rhi
{
	class Device;

	class CONCERTO_GRAPHICS_RHI_BASE_API GpuSubMesh
	{
	public:
		GpuSubMesh(rhi::SubMeshPtr meshPtr, rhi::MaterialInstancePtr material);
		GpuSubMesh(rhi::SubMeshPtr meshPtr, rhi::MaterialInstancePtr material, rhi::Device& device);
		~GpuSubMesh() = default;

		[[nodiscard]] const Vertices& GetVertices() const;
		[[nodiscard]] const rhi::MaterialInstancePtr& GetMaterial() const;
		[[nodiscard]] const rhi::SubMeshPtr& GetSubMesh() const;
		const rhi::Buffer& GetVertexBuffer() const;
		[[nodiscard]] const AABB& GetLocalBounds() const;

		void UploadVertices();

	private:
		void ComputeLocalBounds();

		rhi::SubMeshPtr m_subMesh;
		rhi::MaterialInstancePtr m_material;
		std::unique_ptr<rhi::Buffer> m_vertexBuffer;
		AABB m_localBounds;
	};
	using GpuSubMeshPtr = std::shared_ptr<GpuSubMesh>;
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_GPUSUBMESH_HPP
