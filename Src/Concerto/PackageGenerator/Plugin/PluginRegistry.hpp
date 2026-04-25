//
// Created by arthur - Plugin Registry for Concerto PackageGenerator
//

#ifndef CONCERTO_PKGGENERATOR_PLUGINREGISTRY_HPP
#define CONCERTO_PKGGENERATOR_PLUGINREGISTRY_HPP

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <Concerto/Core/DynLib/DynLib.hpp>

#include "Concerto/PackageGenerator/Defines.hpp"
#include "Concerto/PackageGenerator/Plugin/PluginApi.h"
#include "Concerto/PackageGenerator/Plugin/ReflectionGeneratorPlugin.hpp"

namespace cct
{
	class CCT_PKGGENERATOR_API PluginRegistry
	{
	public:
		struct SortedPlugin
		{
			const CrpPluginFunctionTable* functionTable = nullptr;
			void* privateData = nullptr;
		};

		PluginRegistry() = default;
		~PluginRegistry();

		PluginRegistry(const PluginRegistry&) = delete;
		PluginRegistry& operator=(const PluginRegistry&) = delete;
		PluginRegistry(PluginRegistry&&) noexcept = default;
		PluginRegistry& operator=(PluginRegistry&&) noexcept = default;

		bool LoadPlugin(std::string_view path);
		bool LoadPluginsFromDirectory(std::string_view path);

		void UnloadAll();

		[[nodiscard]] std::span<const SortedPlugin> GetPlugins() const;

		void TransformPackage(Package& package) const;

		void InvokeBeforePackageGeneration(GenerationContext& ctx) const;
		void InvokeOnPackageGeneration(GenerationContext& ctx) const;
		void InvokeAfterPackageGeneration(GenerationContext& ctx) const;

		void InvokeBeforeNamespaceGeneration(const Namespace& ns, GenerationContext& ctx) const;
		void InvokeOnNamespaceGeneration(const Namespace& ns, GenerationContext& ctx) const;
		void InvokeAfterNamespaceGeneration(const Namespace& ns, GenerationContext& ctx) const;

		void InvokeBeforeClassGeneration(const Class& klass, GenerationContext& ctx) const;
		void InvokeOnClassGeneration(const Class& klass, GenerationContext& ctx) const;
		void InvokeAfterClassGeneration(const Class& klass, GenerationContext& ctx) const;

		void InvokeBeforeMemberGeneration(const Class::Member& member, GenerationContext& ctx) const;
		void InvokeOnMemberGeneration(const Class::Member& member, GenerationContext& ctx) const;
		void InvokeAfterMemberGeneration(const Class::Member& member, GenerationContext& ctx) const;

		void InvokeBeforeMethodGeneration(const Class::Method& method, GenerationContext& ctx) const;
		void InvokeOnMethodGeneration(const Class::Method& method, GenerationContext& ctx) const;
		void InvokeAfterMethodGeneration(const Class::Method& method, GenerationContext& ctx) const;

		void InvokeBeforeEnumGeneration(const Enum& enumeration, GenerationContext& ctx) const;
		void InvokeOnEnumGeneration(const Enum& enumeration, GenerationContext& ctx) const;
		void InvokeAfterEnumGeneration(const Enum& enumeration, GenerationContext& ctx) const;

		void InvokeBeforeEnumElementGeneration(const Enum::Element& value, GenerationContext& ctx) const;
		void InvokeOnEnumElementGeneration(const Enum::Element& value, GenerationContext& ctx) const;
		void InvokeAfterEnumElementGeneration(const Enum::Element& value, GenerationContext& ctx) const;

		void InvokeBeforeTemplateClassGeneration(const Class& klass, GenerationContext& ctx) const;
		void InvokeOnTemplateClassGeneration(const Class& klass, GenerationContext& ctx) const;
		void InvokeAfterTemplateClassGeneration(const Class& klass, GenerationContext& ctx) const;

		void InvokeBeforeTemplateSpecializationGeneration(const Class& klass, const std::string& specialization, GenerationContext& ctx) const;
		void InvokeOnTemplateSpecializationGeneration(const Class& klass, const std::string& specialization, GenerationContext& ctx) const;
		void InvokeAfterTemplateSpecializationGeneration(const Class& klass, const std::string& specialization, GenerationContext& ctx) const;

		void InvokeBeforeGenericClassGeneration(const Class& klass, GenerationContext& ctx) const;
		void InvokeOnGenericClassGeneration(const Class& klass, GenerationContext& ctx) const;
		void InvokeAfterGenericClassGeneration(const Class& klass, GenerationContext& ctx) const;

		bool HandleAnnotation(
			std::string_view annotationType,
			std::string_view entityName,
			const TomlAttributes& attributes,
			GenerationContext& ctx) const;

	private:
		struct LoadedPlugin
		{
			cct::DynLib library;
			const CrpPluginFunctionTable* functionTable = nullptr;
			void* privateData = nullptr;
		};

		std::vector<LoadedPlugin> m_loadedPlugins;
		std::vector<SortedPlugin> m_sortedPlugins;

		void SortPluginsByPriority();
	};

} // namespace cct

#endif // CONCERTO_PKGGENERATOR_PLUGINREGISTRY_HPP
