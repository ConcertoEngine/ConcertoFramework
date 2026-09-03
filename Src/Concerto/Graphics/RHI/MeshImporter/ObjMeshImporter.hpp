#ifndef CONCERTO_GRAPHICS_RHI_OBJMESHIMPORTER_HPP
#define CONCERTO_GRAPHICS_RHI_OBJMESHIMPORTER_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/MeshImporter/MeshImporter.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API ObjMeshImporter final : public MeshImporter
	{
	public:
		[[nodiscard]] bool CanImport(const std::string& filePath) const override;
		[[nodiscard]] bool Import(const std::string& filePath, ImportedMesh& outMesh) const override;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_OBJMESHIMPORTER_HPP
