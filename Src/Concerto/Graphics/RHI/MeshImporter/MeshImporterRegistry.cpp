#include "Concerto/Graphics/RHI/MeshImporter/MeshImporterRegistry.hpp"

#include "Concerto/Graphics/RHI/MeshImporter/ObjMeshImporter.hpp"

namespace cct::gfx::rhi
{
	MeshImporterRegistry::MeshImporterRegistry()
	{
		m_importers.push_back(std::make_unique<ObjMeshImporter>());
	}

	void MeshImporterRegistry::RegisterImporter(MeshImporterPtr importer)
	{
		m_importers.push_back(std::move(importer));
	}

	MeshImporter* MeshImporterRegistry::FindImporter(const std::string& filePath) const
	{
		for (auto it = m_importers.rbegin(); it != m_importers.rend(); ++it)
		{
			if ((*it)->CanImport(filePath))
				return it->get();
		}
		return nullptr;
	}
} // namespace cct::gfx::rhi
