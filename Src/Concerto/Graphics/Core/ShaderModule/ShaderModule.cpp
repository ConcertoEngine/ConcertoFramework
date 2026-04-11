//
// Created by arthur on 23/10/2025.
//

#include "Concerto/Graphics/Core/ShaderModule/ShaderModule.hpp"

namespace cct::gfx
{
	ShaderModule::ShaderModule(std::vector<UInt32> shaderBytes,
		std::unordered_map<UInt32, std::vector<DescriptorSetLayoutBinding>> bindings,
		std::string entryPointName,
		ShaderStage stage) :
		m_shaderBytes(std::move(shaderBytes)),
		m_bindings(std::move(bindings)),
		m_entryPointName(std::move(entryPointName)),
		m_stage(stage)
	{
	}

	const std::vector<UInt32>& ShaderModule::GetShaderBytes() const
	{
		return m_shaderBytes;
	}

	const std::unordered_map<UInt32, std::vector<DescriptorSetLayoutBinding>>& ShaderModule::GetBindings() const
	{
		return m_bindings;
	}

	const std::string& ShaderModule::GetEntryPointName() const
	{
		return m_entryPointName;
	}

	ShaderStage ShaderModule::GetStage() const
	{
		return m_stage;
	}
}
