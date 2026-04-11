//
// Created by arthur on 18/02/2023.
//

#ifndef CONCERTO_GRAPHICS_RHI_SUBMESH_HPP
#define CONCERTO_GRAPHICS_RHI_SUBMESH_HPP

#include <memory>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Material.hpp"
#include "Concerto/Graphics/Core/Vertex.hpp"
#include "Concerto/Graphics/RHI/Mesh/Mesh.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API SubMesh
	{
	 public:
		explicit SubMesh(Mesh* parent);
		SubMesh(const Vertices& vertices, std::shared_ptr<rhi::MaterialInfo>& material, Mesh* parent);
		[[nodiscard]] Vertices& GetVertices();
		[[nodiscard]] std::shared_ptr<rhi::MaterialInfo>& GetMaterial();
		[[nodiscard]] Mesh* GetParent();
	 private:
		Vertices m_vertices;
		std::shared_ptr<rhi::MaterialInfo> m_material;
		Mesh* m_parent;
	};
	using SubMeshPtr = std::shared_ptr<SubMesh>;
}
#endif //CONCERTO_GRAPHICS_RHI_SUBMESH_HPP
