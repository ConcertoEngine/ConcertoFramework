//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_SHADERMODULE_HPP
#define CONCERTO_GRAPHICS_RHI_SHADERMODULE_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "Concerto/Graphics/Core/ShaderModule/ShaderModule.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	/**
	 * @brief RHI wrapper for shader modules - API agnostic interface
	 */
	class CONCERTO_GRAPHICS_RHI_BASE_API ShaderModule
	{
	public:
		virtual ~ShaderModule() = default;

		/**
		 * @brief Get the shader bytecode
		 */
		virtual const std::vector<UInt32>& GetShaderBytes() const = 0;

		/**
		 * @brief Get shader bindings organized by descriptor set
		 */
		virtual const std::unordered_map<UInt32, std::vector<cct::gfx::DescriptorSetLayoutBinding>>& GetBindings() const = 0;

		/**
		 * @brief Get the entry point name of the shader
		 */
		virtual const std::string& GetEntryPointName() const = 0;

		/**
		 * @brief Get the shader stage (vertex, fragment, compute)
		 */
		virtual cct::gfx::ShaderStage GetStage() const = 0;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_SHADERMODULE_HPP
