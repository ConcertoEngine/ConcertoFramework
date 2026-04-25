//
// Created by arthur - Plugin Registry Implementation
//

#include "Concerto/PackageGenerator/Plugin/PluginRegistry.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <Concerto/Core/Logger/Logger.hpp>
#include <Concerto/Profiler/Profiler.hpp>

#include "Concerto/PackageGenerator/Plugin/ReflectionGeneratorPlugin.hpp"

namespace cct
{
	PluginRegistry::~PluginRegistry()
	{
		UnloadAll();
	}

	bool PluginRegistry::LoadPlugin(std::string_view path)
	{
		CCT_AUTO_PROFILER_SCOPE();
		LoadedPlugin plugin;

		if (!plugin.library.Load(path))
		{
			Logger::Error("Failed to load plugin: {}", path);
			return false;
		}

		// Get the plugin entry point
		auto getTableFunc = plugin.library.GetFunction<const CrpPluginFunctionTable*>(
			CRP_PLUGIN_ENTRY_POINT_NAME);
		if (!getTableFunc)
		{
			Logger::Error("Plugin missing entry point '{}': {}", CRP_PLUGIN_ENTRY_POINT_NAME, path);
			return false;
		}

		plugin.functionTable = getTableFunc();
		if (!plugin.functionTable)
		{
			Logger::Error("Plugin entry point returned nullptr: {}", path);
			return false;
		}

		// Check API version
		if (plugin.functionTable->apiVersion != CRP_PLUGIN_API_VERSION)
		{
			Logger::Error("Plugin API version mismatch: expected {}, got {} for plugin: {}",
						  CRP_PLUGIN_API_VERSION, plugin.functionTable->apiVersion, path);
			return false;
		}

		// GetInfo is required
		if (!plugin.functionTable->GetInfo)
		{
			Logger::Error("Plugin missing required GetInfo function: {}", path);
			return false;
		}

		// Call OnLoad if provided
		if (plugin.functionTable->OnLoad)
		{
			if (!plugin.functionTable->OnLoad(&plugin.privateData))
			{
				Logger::Error("Plugin OnLoad failed: {}", path);
				return false;
			}
		}

		// Log plugin info
		CrpPluginInfo info{};
		plugin.functionTable->GetInfo(&info);
		Logger::Info("Loaded plugin: {} v{} by {}",
					 info.name ? info.name : "unknown",
					 info.version ? info.version : "unknown",
					 info.author ? info.author : "unknown");

		m_loadedPlugins.push_back(std::move(plugin));
		SortPluginsByPriority();

		return true;
	}

	bool PluginRegistry::LoadPluginsFromDirectory(std::string_view path)
	{
		namespace fs = std::filesystem;

		if (!fs::exists(path) || !fs::is_directory(path))
		{
			Logger::Warning("Plugin directory does not exist: {}", path);
			return false;
		}

		bool allLoaded = true;
		for (const auto& entry : fs::directory_iterator(path))
		{
			if (!entry.is_regular_file())
				continue;

			auto ext = entry.path().extension();
			if (ext != CONCERTO_DYNLIB_EXTENSION)
				continue;

			if (!LoadPlugin(entry.path().string()))
				allLoaded = false;
		}

		return allLoaded;
	}

	std::span<const PluginRegistry::SortedPlugin> PluginRegistry::GetPlugins() const
	{
		return m_sortedPlugins;
	}

	void PluginRegistry::UnloadAll()
	{
		// Call OnUnload for all plugins in reverse order
		for (auto it = m_loadedPlugins.rbegin(); it != m_loadedPlugins.rend(); ++it)
		{
			if (it->functionTable && it->functionTable->OnUnload)
			{
				it->functionTable->OnUnload(it->privateData);
			}
		}
		m_loadedPlugins.clear();
		m_sortedPlugins.clear();
	}

	void PluginRegistry::TransformPackage(Package& package) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->TransformPackage)
				entry.functionTable->TransformPackage(reinterpret_cast<CrpPackage*>(&package));
		}
	}

	void PluginRegistry::InvokeBeforePackageGeneration(GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforePackageGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforePackageGeneration(reinterpret_cast<const CrpPackage*>(ctx.package), reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnPackageGeneration(GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnPackageGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnPackageGeneration(reinterpret_cast<const CrpPackage*>(ctx.package), reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterPackageGeneration(GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterPackageGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterPackageGeneration(reinterpret_cast<const CrpPackage*>(ctx.package), reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeBeforeNamespaceGeneration(const Namespace& ns, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforeNamespaceGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforeNamespaceGeneration(
					reinterpret_cast<const CrpNamespace*>(&ns),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnNamespaceGeneration(const Namespace& ns, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnNamespaceGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnNamespaceGeneration(
					reinterpret_cast<const CrpNamespace*>(&ns),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterNamespaceGeneration(const Namespace& ns, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterNamespaceGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterNamespaceGeneration(
					reinterpret_cast<const CrpNamespace*>(&ns),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeBeforeClassGeneration(const Class& klass, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforeClassGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforeClassGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnClassGeneration(const Class& klass, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnClassGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnClassGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterClassGeneration(const Class& klass, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterClassGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterClassGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeBeforeMemberGeneration(const Class::Member& member, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforeMemberGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforeMemberGeneration(
					reinterpret_cast<const CrpClassMember*>(&member),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnMemberGeneration(const Class::Member& member, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnMemberGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnMemberGeneration(
					reinterpret_cast<const CrpClassMember*>(&member),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterMemberGeneration(const Class::Member& member, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterMemberGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterMemberGeneration(
					reinterpret_cast<const CrpClassMember*>(&member),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeBeforeMethodGeneration(const Class::Method& method, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforeMethodGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforeMethodGeneration(
					reinterpret_cast<const CrpClassMethod*>(&method),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnMethodGeneration(const Class::Method& method, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnMethodGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnMethodGeneration(
					reinterpret_cast<const CrpClassMethod*>(&method),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterMethodGeneration(const Class::Method& method, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterMethodGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterMethodGeneration(
					reinterpret_cast<const CrpClassMethod*>(&method),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeBeforeEnumGeneration(const Enum& enumeration, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforeEnumGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforeEnumGeneration(
					reinterpret_cast<const CrpEnum*>(&enumeration),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnEnumGeneration(const Enum& enumeration, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnEnumGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnEnumGeneration(
					reinterpret_cast<const CrpEnum*>(&enumeration),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterEnumGeneration(const Enum& enumeration, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterEnumGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterEnumGeneration(
					reinterpret_cast<const CrpEnum*>(&enumeration),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeBeforeEnumElementGeneration(const Enum::Element& element,
														   GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforeEnumElementGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforeEnumElementGeneration(
					reinterpret_cast<const CrpEnumElement*>(&element),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnEnumElementGeneration(const Enum::Element& element, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnEnumElementGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnEnumElementGeneration(
					reinterpret_cast<const CrpEnumElement*>(&element),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterEnumElementGeneration(const Enum::Element& element,
														  GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterEnumElementGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterEnumElementGeneration(
					reinterpret_cast<const CrpEnumElement*>(&element),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeBeforeTemplateClassGeneration(const Class& klass, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforeTemplateClassGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforeTemplateClassGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnTemplateClassGeneration(const Class& klass, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnTemplateClassGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnTemplateClassGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterTemplateClassGeneration(const Class& klass, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterTemplateClassGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterTemplateClassGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeBeforeTemplateSpecializationGeneration(const Class& klass, const std::string& specialization, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforeTemplateSpecializationGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforeTemplateSpecializationGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					specialization.c_str(),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnTemplateSpecializationGeneration(const Class& klass, const std::string& specialization, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnTemplateSpecializationGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnTemplateSpecializationGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					specialization.c_str(),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterTemplateSpecializationGeneration(const Class& klass, const std::string& specialization, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterTemplateSpecializationGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterTemplateSpecializationGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					specialization.c_str(),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeBeforeGenericClassGeneration(const Class& klass, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->BeforeGenericClassGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->BeforeGenericClassGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeOnGenericClassGeneration(const Class& klass, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->OnGenericClassGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->OnGenericClassGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	void PluginRegistry::InvokeAfterGenericClassGeneration(const Class& klass, GenerationContext& ctx) const
	{
		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->AfterGenericClassGeneration)
			{
				ctx.pluginPrivateData = entry.privateData;
				entry.functionTable->AfterGenericClassGeneration(
					reinterpret_cast<const CrpClass*>(&klass),
					reinterpret_cast<CrpGenerationContext*>(&ctx));
			}
		}
	}

	bool PluginRegistry::HandleAnnotation(
		std::string_view annotationType,
		std::string_view entityName,
		const TomlAttributes& attributes,
		GenerationContext& ctx) const
	{
		std::string typeStr(annotationType);
		std::string nameStr(entityName);

		for (const auto& entry : m_sortedPlugins)
		{
			if (entry.functionTable->HandleAnnotation)
			{
				ctx.pluginPrivateData = entry.privateData;
				if (entry.functionTable->HandleAnnotation(
						typeStr.c_str(),
						nameStr.c_str(),
						reinterpret_cast<const CrpTomlAttributes*>(&attributes),
						reinterpret_cast<CrpGenerationContext*>(&ctx)))
					return true;
			}
		}
		return false;
	}

	void PluginRegistry::SortPluginsByPriority()
	{
		m_sortedPlugins.clear();
		for (auto& loaded : m_loadedPlugins)
			m_sortedPlugins.push_back({loaded.functionTable, loaded.privateData});

		std::ranges::sort(m_sortedPlugins, [](const auto& a, const auto& b)
						  {
			int32_t priorityA = a.functionTable->GetPriority ? a.functionTable->GetPriority() : 0;
			int32_t priorityB = b.functionTable->GetPriority ? b.functionTable->GetPriority() : 0;
			return priorityA > priorityB; });
	}

} // namespace cct
