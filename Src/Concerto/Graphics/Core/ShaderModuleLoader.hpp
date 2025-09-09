//
// Created by arthur on 23/10/2025.
//

#ifndef CONCERTO_GRAPHICS_CORE_SHADERMODULELOADER_HPP
#define CONCERTO_GRAPHICS_CORE_SHADERMODULELOADER_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <NZSL/Ast/Module.hpp>

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/Core/ShaderModule.hpp"

namespace cct::gfx
{
	struct CONCERTO_GRAPHICS_CORE_API ResolvedShaderModule
	{
		nzsl::Ast::ModulePtr resolvedAst;
		std::unordered_map<UInt32, std::vector<DescriptorSetLayoutBinding>> bindings;
		std::string entryPointName;
		ShaderStage stage;
	};

	class CONCERTO_GRAPHICS_CORE_API ShaderModuleLoader
	{
	public:
		ShaderModuleLoader() = default;
		ShaderModuleLoader(const ShaderModuleLoader&) = delete;
		ShaderModuleLoader(ShaderModuleLoader&&) = default;

		ShaderModuleLoader& operator=(const ShaderModuleLoader&) = delete;
		ShaderModuleLoader& operator=(ShaderModuleLoader&&) = default;

		ResolvedShaderModule ResolveShaderModule(const std::string& path);
		ShaderModule LoadShaderModule(const std::string& path);

	private:
		static ShaderBindingType GetBindingType(const nzsl::Ast::ExpressionType* varType);
	};
}

#endif //CONCERTO_GRAPHICS_CORE_SHADERMODULELOADER_HPP
