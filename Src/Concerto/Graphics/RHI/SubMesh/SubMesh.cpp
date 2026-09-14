//
// Created by arthur on 18/02/2023.
//

#include "Concerto/Graphics/RHI/SubMesh/SubMesh.hpp"

#include <cstring>
#include <string_view>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ShaderModule/ShaderModule.hpp"
#include "Concerto/Graphics/RHI/Mesh/Mesh.hpp"

namespace cct::gfx::rhi
{
	namespace
	{
		static_assert(sizeof(Vertex) == 11 * sizeof(float),
					  "Vertex layout changed: BuildIndices hashes and compares raw bytes, which is only valid while Vertex has no padding");

		struct VertexHasher
		{
			std::size_t operator()(const Vertex& v) const noexcept
			{
				return std::hash<std::string_view>{}(std::string_view(reinterpret_cast<const char*>(&v), sizeof(Vertex)));
			}
		};

		struct VertexEqual
		{
			bool operator()(const Vertex& a, const Vertex& b) const noexcept
			{
				return std::memcmp(&a, &b, sizeof(Vertex)) == 0;
			}
		};
	} // namespace

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
		BuildIndices();
	}

	const Vertices& SubMesh::GetVertices() const
	{
		return m_vertices;
	}

	const Indices& SubMesh::GetIndices() const
	{
		return m_indices;
	}

	bool SubMesh::HasIndices() const
	{
		return !m_indices.empty();
	}

	std::shared_ptr<rhi::MaterialInfo>& SubMesh::GetMaterial()
	{
		return m_material;
	}

	Mesh* SubMesh::GetParent()
	{
		return m_parent;
	}

	void SubMesh::SetVertices(Vertices vertices)
	{
		m_vertices = std::move(vertices);
		m_indices.clear();
		BuildIndices();
	}

	void SubMesh::SetGeometry(Vertices vertices, Indices indices)
	{
		m_vertices = std::move(vertices);
		m_indices = std::move(indices);
		if (m_indices.empty())
			BuildIndices();
	}

	void SubMesh::BuildIndices()
	{
		if (m_vertices.empty())
			return;

		Vertices uniqueVertices;
		Indices indices;
		uniqueVertices.reserve(m_vertices.size());
		indices.reserve(m_vertices.size());

		phmap::flat_hash_map<Vertex, Index, VertexHasher, VertexEqual> vertexToIndex;
		vertexToIndex.reserve(m_vertices.size());

		for (const Vertex& vertex : m_vertices)
		{
			auto [it, inserted] = vertexToIndex.try_emplace(vertex, static_cast<Index>(uniqueVertices.size()));
			if (inserted)
				uniqueVertices.push_back(vertex);
			indices.push_back(it->second);
		}

		m_vertices = std::move(uniqueVertices);
		m_indices = std::move(indices);
	}
} // namespace cct::gfx::rhi
