#define TINYOBJLOADER_IMPLEMENTATION

#include "Concerto/Graphics/RHI/MeshImporter/ObjMeshImporter.hpp"

#include <filesystem>
#include <tiny_obj_loader.h>

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	namespace
	{
		struct ObjIndexKey
		{
			int position;
			int normal;
			int texCoord;

			bool operator==(const ObjIndexKey& other) const noexcept
			{
				return position == other.position && normal == other.normal && texCoord == other.texCoord;
			}
		};

		struct ObjIndexKeyHasher
		{
			std::size_t operator()(const ObjIndexKey& key) const noexcept
			{
				std::size_t hash = static_cast<std::size_t>(static_cast<UInt32>(key.position));
				hash = hash * 31 + static_cast<std::size_t>(static_cast<UInt32>(key.normal));
				hash = hash * 31 + static_cast<std::size_t>(static_cast<UInt32>(key.texCoord));
				return hash;
			}
		};
	} // namespace

	bool ObjMeshImporter::CanImport(const std::string& filePath) const
	{
		std::filesystem::path path(filePath);
		return path.extension() == ".obj";
	}

	bool ObjMeshImporter::Import(const std::string& filePath, ImportedMesh& outMesh) const
	{
		tinyobj::ObjReaderConfig readerConfig;
		tinyobj::ObjReader reader;

		if (!reader.ParseFromFile(filePath, readerConfig))
		{
			if (!reader.Error().empty())
				CCT_RHI_LOG_WARN("TinyObjReader: {}", reader.Error());
			return false;
		}

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();

		std::filesystem::path path = std::filesystem::path(filePath).parent_path();
		for (auto& material : materials)
		{
			std::shared_ptr<rhi::MaterialInfo> mat = std::make_shared<rhi::MaterialInfo>();
			std::string diffuse_texname = material.diffuse_texname;
			std::string normal_texname = material.normal_texname;
#ifdef CCT_PLATFORM_POSIX
			auto cleanPath = [](std::string& p)
			{
				std::replace(p.begin(), p.end(), '\\', '/');
			};
			cleanPath(diffuse_texname);
			cleanPath(normal_texname);
#endif // CCT_PLATFORM_POSIX

			mat->diffuseTexturePath = diffuse_texname.empty() ? "" : (path / diffuse_texname).string();
			mat->normalTexturePath = normal_texname.empty() ? "" : (path / normal_texname).string();
			mat->diffuseColor.X() = material.diffuse[0];
			mat->diffuseColor.Y() = material.diffuse[1];
			mat->diffuseColor.Z() = material.diffuse[2];
			mat->metallic = material.metallic;
			mat->specular.X() = material.specular[0];
			mat->specular.Y() = material.specular[1];
			mat->specular.Z() = material.specular[2];
			mat->roughness = material.roughness;
			mat->anisotropy = material.anisotropy;
			mat->emissiveColor.X() = material.emission[0];
			mat->emissiveColor.Y() = material.emission[1];
			mat->emissiveColor.Z() = material.emission[2];
			mat->name = material.name;
			outMesh.materials[material.name] = std::move(mat);
		}

		int currentSubMeshIndex = -1;
		phmap::flat_hash_map<ObjIndexKey, Index, ObjIndexKeyHasher> vertexLookup;
		for (const auto& shape : shapes)
		{
			std::size_t index_offset = 0;
			const int fv = 3;
			for (std::size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
			{
				const int matId = shape.mesh.material_ids[f];
				if (currentSubMeshIndex == -1 || outMesh.subMeshes[currentSubMeshIndex].materialName != materials[matId].name)
				{
					outMesh.subMeshes.push_back(ImportedSubMesh{.materialName = materials[matId].name});
					currentSubMeshIndex++;
					vertexLookup.clear();
				}

				ImportedSubMesh& currentSubMesh = outMesh.subMeshes[currentSubMeshIndex];

				for (std::size_t v = 0; v < fv; v++)
				{
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					const ObjIndexKey key{idx.vertex_index, idx.normal_index, idx.texcoord_index};
					auto [it, inserted] = vertexLookup.try_emplace(key, static_cast<Index>(currentSubMesh.vertices.size()));
					if (inserted)
					{
						tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
						tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
						tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

						tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
						tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
						tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

						tinyobj::real_t ux = attrib.texcoords[2 * idx.texcoord_index + 0];
						tinyobj::real_t uy = attrib.texcoords[2 * idx.texcoord_index + 1];

						currentSubMesh.vertices.emplace_back(Vertex{Vector3f{vx, vy, vz},
																	Vector3f{nx, ny, nz},
																	Vector3f{nx, ny, nz},
																	Vector2f{ux, 1 - uy}});
					}
					currentSubMesh.indices.push_back(it->second);
				}
				index_offset += fv;
			}
		}
		return true;
	}
} // namespace cct::gfx::rhi
