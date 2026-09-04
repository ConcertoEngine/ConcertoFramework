//
// Created by arthur on 17/02/2023.
//

#include "Concerto/Graphics/RHI/Mesh/Mesh.hpp"

#include <Concerto/Core/Cast.hpp>
#include <Concerto/Core/Logger/Logger.hpp>
#include <Concerto/Core/ThreadPool/ThreadPool.hpp>

#include "Concerto/Graphics/RHI/Device.hpp"
#include "Concerto/Graphics/RHI/GpuMesh.hpp"
#include "Concerto/Graphics/RHI/GpuSubMesh/GpuSubMesh.hpp"
#include "Concerto/Graphics/RHI/Material.hpp"
#include "Concerto/Graphics/RHI/MaterialBuilder.hpp"
#include "Concerto/Graphics/RHI/MeshImporter/MeshImporter.hpp"
#include "Concerto/Graphics/RHI/SubMesh/SubMesh.hpp"
#include "Concerto/Graphics/RHI/TextureBuilder/TextureBuilder.hpp"

namespace cct::gfx::rhi
{
	Mesh::Mesh(std::string filePath, MeshImporterRegistry importerRegistry) :
		m_path(std::move(filePath)),
		m_importerRegistry(std::move(importerRegistry))
	{
		bool loaded = LoadFromFile(m_path);
		CCT_ASSERT(loaded, "LoadFromFile failed");
	}

	Mesh::Mesh(Vertices vertices)
	{
		rhi::SubMeshPtr subMesh = std::make_shared<rhi::SubMesh>(this);
		subMesh->GetVertices() = std::move(vertices);
		m_subMeshes.push_back(subMesh);
	}

	std::vector<rhi::SubMeshPtr>& Mesh::GetSubMeshes()
	{
		return m_subMeshes;
	}

	bool Mesh::LoadFromFile(const std::string& fileName)
	{
		CCT_AUTO_PROFILER_SCOPE();

		MeshImporter* importer = m_importerRegistry.FindImporter(fileName);
		if (importer == nullptr)
		{
			CCT_RHI_LOG_ERROR("No mesh importer found for file '{}'", fileName);
			return false;
		}

		ImportedMesh imported;
		if (!importer->Import(fileName, imported))
			return false;

		m_materials = std::move(imported.materials);
		for (auto& importedSubMesh : imported.subMeshes)
		{
			SubMeshPtr subMesh = std::make_shared<SubMesh>(this);
			subMesh->GetVertices() = std::move(importedSubMesh.vertices);
			auto it = m_materials.find(importedSubMesh.materialName);
			if (it != m_materials.end())
				subMesh->GetMaterial() = it->second;
			m_subMeshes.push_back(std::move(subMesh));
		}
		return true;
	}

	const std::string& Mesh::GetPath() const
	{
		return m_path;
	}

	std::unordered_map<std::string, std::shared_ptr<rhi::MaterialInfo>>& Mesh::GetMaterials()
	{
		return m_materials;
	}

	std::unique_ptr<GpuMesh> Mesh::BuildGpuMesh(rhi::MaterialBuilder& materialBuilder, rhi::TextureBuilder& textureBuilder, const rhi::RenderPass& renderPass, rhi::Device& device)
	{
		auto gpuMesh = std::make_unique<rhi::GpuMesh>();
		auto& meshes = GetSubMeshes();

		std::atomic<std::size_t> totalVertices = 0;
		{
			for (auto& subMesh : meshes)
			{
				// threadPool.AddTask([&](){
				auto& materialInfo = *subMesh->GetMaterial();
				materialInfo.vertexShaderPath = "./Shaders/tri_mesh_ssbo.nzsl";
				materialInfo.fragmentShaderPath = materialInfo.diffuseTexturePath.empty()
													  ? "./Shaders/default_lit.nzsl"
													  : "./Shaders/textured_lit.nzsl";

				rhi::MaterialInstancePtr material = materialBuilder.BuildMaterial(materialInfo, renderPass);
				auto gpuSubMesh = std::make_shared<GpuSubMesh>(subMesh, material, device);
				// std::scoped_lock m_(subMeshesMutex);
				gpuMesh->subMeshes.push_back(gpuSubMesh);
				//});
			}
		}
		// Sort by material for batch rendering
		{
			phmap::flat_hash_map<std::size_t, std::vector<GpuSubMeshPtr>> subMeshesByMaterial;
			for (auto& subMesh : gpuMesh->subMeshes)
			{
				auto hash = subMesh->GetMaterial()->GetHash();
				auto it = subMeshesByMaterial.find(hash);
				if (it == subMeshesByMaterial.end())
					subMeshesByMaterial.emplace(hash, std::vector{subMesh});
				else
					it->second.emplace_back(subMesh);
			}

			gpuMesh->subMeshes.clear();
			for (auto& [hash, subMeshes] : subMeshesByMaterial)
			{
				for (auto& subMesh : subMeshes)
					gpuMesh->subMeshes.emplace_back(subMesh);
			}
		}

		textureBuilder.Commit();

		for (auto& gpuSubMesh : gpuMesh->subMeshes)
			gpuSubMesh->UploadVertices();

		return gpuMesh;
	}
} // namespace cct::gfx::rhi
