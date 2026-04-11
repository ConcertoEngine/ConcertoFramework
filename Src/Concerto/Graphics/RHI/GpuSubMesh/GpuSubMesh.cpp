//
// Created by arthur on 09/06/22.
//

#include <cstring>

#include "Concerto/Graphics/RHI/GpuSubMesh/GpuSubMesh.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"
#include "Concerto/Graphics/Core/Vertex.hpp"

namespace cct::gfx::rhi
{
	GpuSubMesh::GpuSubMesh(rhi::SubMeshPtr meshPtr, rhi::MaterialPtr material) :
		m_subMesh(std::move(meshPtr)),
		m_material(std::move(material))
	{
	}

	GpuSubMesh::GpuSubMesh(rhi::SubMeshPtr meshPtr, rhi::MaterialPtr material, rhi::Device& device) :
		m_subMesh(std::move(meshPtr)),
		m_material(std::move(material))
	{
		m_vertexBuffer = device.CreateBuffer(
			static_cast<BufferUsageFlags>(BufferUsage::Vertex),
			static_cast<UInt32>(m_subMesh->GetVertices().size() * sizeof(Vertex)),
			true);
	}

	const Vertices& GpuSubMesh::GetVertices() const
	{
		return m_subMesh->GetVertices();
	}

	const rhi::MaterialPtr& GpuSubMesh::GetMaterial() const
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
	}
} // cct::gfx::rhi