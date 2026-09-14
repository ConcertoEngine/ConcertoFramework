//
// Created by arthur on 09/06/22.
//

#include "Concerto/Graphics/RHI/GpuSubMesh/GpuSubMesh.hpp"

#include <cstring>
#include <limits>

#include "Concerto/Graphics/Core/Vertex.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"

namespace cct::gfx::rhi
{
	GpuSubMesh::GpuSubMesh(rhi::SubMeshPtr meshPtr, rhi::MaterialInstancePtr material) :
		m_subMesh(std::move(meshPtr)),
		m_material(std::move(material))
	{
		ComputeLocalBounds();
	}

	GpuSubMesh::GpuSubMesh(rhi::SubMeshPtr meshPtr, rhi::MaterialInstancePtr material, rhi::Device& device) :
		m_subMesh(std::move(meshPtr)),
		m_material(std::move(material))
	{
		ComputeLocalBounds();
		m_vertexBuffer = device.CreateBuffer(
			BufferUsage::Vertex,
			static_cast<UInt32>(m_subMesh->GetVertices().size() * sizeof(Vertex)),
			true);
		if (m_subMesh->HasIndices())
		{
			m_indexCount = static_cast<UInt32>(m_subMesh->GetIndices().size());
			m_use32BitIndices = m_subMesh->GetVertices().size() > std::numeric_limits<UInt16>::max();
			m_indexBuffer = device.CreateBuffer(
				BufferUsage::Index,
				static_cast<UInt32>(m_indexCount * (m_use32BitIndices ? sizeof(UInt32) : sizeof(UInt16))),
				true);
		}
	}

	const Vertices& GpuSubMesh::GetVertices() const
	{
		return m_subMesh->GetVertices();
	}

	const rhi::MaterialInstancePtr& GpuSubMesh::GetMaterial() const
	{
		return m_material;
	}

	const rhi::SubMeshPtr& GpuSubMesh::GetSubMesh() const
	{
		return m_subMesh;
	}

	const rhi::Buffer& GpuSubMesh::GetVertexBuffer() const
	{
		CCT_ASSERT(m_vertexBuffer, "ConcertoGraphics: invalid vertex buffer");
		return *m_vertexBuffer;
	}

	bool GpuSubMesh::HasIndexBuffer() const
	{
		return m_indexBuffer != nullptr;
	}

	const rhi::Buffer& GpuSubMesh::GetIndexBuffer() const
	{
		CCT_ASSERT(m_indexBuffer, "ConcertoGraphics: invalid index buffer");
		return *m_indexBuffer;
	}

	UInt32 GpuSubMesh::GetIndexCount() const
	{
		return m_indexCount;
	}

	bool GpuSubMesh::UsesUInt32Indices() const
	{
		return m_use32BitIndices;
	}

	const AABB& GpuSubMesh::GetLocalBounds() const
	{
		return m_localBounds;
	}

	void GpuSubMesh::UploadVertices()
	{
		CCT_ASSERT(m_vertexBuffer, "ConcertoGraphics: invalid vertex buffer");
		Byte* data = nullptr;
		if (m_vertexBuffer->Map(&data))
		{
			auto& vertices = m_subMesh->GetVertices();
			std::memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
			m_vertexBuffer->UnMap();
		}

		if (m_indexBuffer != nullptr)
		{
			const Indices& indices = m_subMesh->GetIndices();
			CCT_ASSERT(indices.size() == m_indexCount, "ConcertoGraphics: index count changed after buffer creation");
			Byte* indexData = nullptr;
			if (m_indexBuffer->Map(&indexData))
			{
				if (m_use32BitIndices)
					std::memcpy(indexData, indices.data(), indices.size() * sizeof(UInt32));
				else
				{
					UInt16* narrowedIndices = reinterpret_cast<UInt16*>(indexData);
					for (std::size_t i = 0; i < indices.size(); ++i)
						narrowedIndices[i] = static_cast<UInt16>(indices[i]);
				}
				m_indexBuffer->UnMap();
			}
		}
	}

	void GpuSubMesh::ComputeLocalBounds()
	{
		for (const Vertex& vertex : m_subMesh->GetVertices())
			m_localBounds.Extend(vertex.position);
	}
} // namespace cct::gfx::rhi