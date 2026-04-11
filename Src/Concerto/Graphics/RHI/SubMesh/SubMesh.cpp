//
// Created by arthur on 18/02/2023.
//

#include "Concerto/Graphics/RHI/SubMesh/SubMesh.hpp"
#include "Concerto/Graphics/RHI/Mesh/Mesh.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ShaderModule/ShaderModule.hpp"

namespace cct::gfx::rhi
{
	SubMesh::SubMesh(Mesh* parent) :
		m_vertices(),
		m_material(std::make_shared<rhi::MaterialInfo>()),
		m_parent(parent)
	{

	}

	SubMesh::SubMesh(const Vertices& vertices, std::shared_ptr<rhi::MaterialInfo>& material, Mesh* parent) :
		m_vertices(vertices),
		m_material(material),
		m_parent(parent)
	{

	}

	Vertices& SubMesh::GetVertices()
	{
		return m_vertices;
	}

	std::shared_ptr<rhi::MaterialInfo>& SubMesh::GetMaterial()
	{
		return m_material;
	}

	Mesh* SubMesh::GetParent()
	{
		return m_parent;
	}
}