#ifndef CONCERTO_GRAPHICS_RHI_MESHIMPORTER_HPP
#define CONCERTO_GRAPHICS_RHI_MESHIMPORTER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Concerto/Graphics/Core/Vertex.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Material.hpp"

namespace cct::gfx::rhi
{
	struct ImportedSubMesh
	{
		Vertices vertices;
		Indices indices;
		std::string materialName;
	};

	struct ImportedMesh
	{
		std::vector<ImportedSubMesh> subMeshes;
		std::unordered_map<std::string, std::shared_ptr<MaterialInfo>> materials;
	};

	class CONCERTO_GRAPHICS_RHI_BASE_API MeshImporter
	{
	public:
		virtual ~MeshImporter() = default;

		[[nodiscard]] virtual bool CanImport(const std::string& filePath) const = 0;
		[[nodiscard]] virtual bool Import(const std::string& filePath, ImportedMesh& outMesh) const = 0;
	};
	using MeshImporterPtr = std::unique_ptr<MeshImporter>;
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_MESHIMPORTER_HPP
