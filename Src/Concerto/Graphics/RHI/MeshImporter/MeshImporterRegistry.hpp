#ifndef CONCERTO_GRAPHICS_RHI_MESHIMPORTERREGISTRY_HPP
#define CONCERTO_GRAPHICS_RHI_MESHIMPORTERREGISTRY_HPP

#include <vector>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/MeshImporter/MeshImporter.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API MeshImporterRegistry
	{
	public:
		MeshImporterRegistry();
		~MeshImporterRegistry() = default;

		MeshImporterRegistry(const MeshImporterRegistry&) = delete;
		MeshImporterRegistry& operator=(const MeshImporterRegistry&) = delete;
		MeshImporterRegistry(MeshImporterRegistry&&) = default;
		MeshImporterRegistry& operator=(MeshImporterRegistry&&) = default;

		void RegisterImporter(MeshImporterPtr importer);
		[[nodiscard]] MeshImporter* FindImporter(const std::string& filePath) const;

	private:
		std::vector<MeshImporterPtr> m_importers;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_MESHIMPORTERREGISTRY_HPP
