//
// Created by arthur on 20/12/2024.
//

#include "Concerto/Reflection/PackageLoader/PackageLoader.hpp"

#include <filesystem>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"
#include "Concerto/Reflection/Namespace/Namespace.inl"
#include "Concerto/Reflection/Package/Package.hpp"

namespace cct::refl
{
	Package* PackageLoader::AddPackage(std::unique_ptr<Package> package)
	{
		Package* packagePtr = package.get();
		m_packages.emplace_back(std::move(package));
		return packagePtr;
	}

	bool PackageLoader::AddDynamicPackage(std::string_view path)
	{
		cct::DynLib lib;
		if (!lib.Load(path))
			return false;
		auto func = lib.GetFunction<std::unique_ptr<Package>>("CreatePackage");
		if (!func)
			return false;
		AddPackage(func());
		m_dynamicPackages.emplace_back(std::move(lib));
		return true;
	}

	bool PackageLoader::AddDynamicFolder(std::string_view path)
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
				continue;
			if (entry.path().extension() != CONCERTO_DYNLIB_EXTENSION)
				continue;
			if (!AddDynamicPackage(entry.path().string()))
				return false;
		}
		return true;
	}

	void PackageLoader::AddPackageAndLoad(std::unique_ptr<Package> package)
	{
		package->LoadNamespaces();
		package->InitializeNamespaces();
		package->InitializeClasses();
		AddPackage(std::move(package));
	}

	bool PackageLoader::AddDynamicPackageAndLoad(std::string_view path)
	{
		if (!AddDynamicPackage(path))
			return false;
		auto& package = m_packages.back();
		package->LoadNamespaces();
		package->InitializeNamespaces();
		package->InitializeClasses();
		return true;
	}

	bool PackageLoader::AddDynamicFolderAndLoad(std::string_view path)
	{
		const std::size_t packageCount = m_packages.size();
		if (!AddDynamicFolder(path))
			return false;
		const std::size_t newPackageCount = m_packages.size();

		for (std::size_t i = packageCount; i < newPackageCount; ++i)
			m_packages[i]->LoadNamespaces();
		for (std::size_t i = packageCount; i < newPackageCount; ++i)
			m_packages[i]->InitializeNamespaces();
		for (std::size_t i = packageCount; i < newPackageCount; ++i)
			m_packages[i]->InitializeClasses();
		return true;
	}

	void PackageLoader::LoadPackages() const
	{
		for (const auto& pkg : m_packages)
			pkg->LoadNamespaces();
		for (const auto& pkg : m_packages)
			pkg->InitializeNamespaces();
		for (const auto& pkg : m_packages)
			pkg->InitializeClasses();
	}
} // namespace cct::refl
