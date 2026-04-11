//
// Created by arthur on 23/10/2025.
//

#ifndef CONCERTO_GRAPHICS_CORE_SHADERMODULE_HPP
#define CONCERTO_GRAPHICS_CORE_SHADERMODULE_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/Core/ShaderTypes.hpp"

namespace cct::gfx
{
	struct CONCERTO_GRAPHICS_CORE_API DescriptorSetLayoutBinding
	{
		UInt32 binding = 0;
		UInt32 descriptorCount = 1;
		ShaderBindingType descriptorType;
		EnumFlags<ShaderStage> stageFlags;
	};

	class CONCERTO_GRAPHICS_CORE_API ShaderModule
	{
	public:
		ShaderModule() = default;
		ShaderModule(std::vector<UInt32> shaderBytes,
			std::unordered_map<UInt32, std::vector<DescriptorSetLayoutBinding>> bindings,
			std::string entryPointName,
			ShaderStage stage);
		ShaderModule(const ShaderModule&) = delete;
		ShaderModule(ShaderModule&&) = default;

		ShaderModule& operator=(const ShaderModule&) = delete;
		ShaderModule& operator=(ShaderModule&&) = default;

		const std::vector<UInt32>& GetShaderBytes() const;
		const std::unordered_map<UInt32, std::vector<DescriptorSetLayoutBinding>>& GetBindings() const;
		const std::string& GetEntryPointName() const;
		ShaderStage GetStage() const;

	private:
		std::vector<UInt32> m_shaderBytes;
		std::unordered_map<UInt32, std::vector<DescriptorSetLayoutBinding>> m_bindings;
		std::string m_entryPointName;
		ShaderStage m_stage;
	};
}

#endif //CONCERTO_GRAPHICS_CORE_SHADERMODULE_HPP
