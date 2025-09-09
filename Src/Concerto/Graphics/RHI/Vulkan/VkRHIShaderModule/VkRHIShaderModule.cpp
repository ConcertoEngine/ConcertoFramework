//
// Created by arthur on 23/10/2025.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIShaderModule/VkRHIShaderModule.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"

namespace cct::gfx::rhi
{
	VkRHIShaderModule::VkRHIShaderModule(vk::Device& device, cct::gfx::ShaderModule&& shaderModule) :
		m_abstractShaderModule(std::move(shaderModule)),
		m_vulkanShaderModule(device,
			m_abstractShaderModule.GetShaderBytes(),
			static_cast<VkShaderStageFlagBits>(Converters::ToVulkan(m_abstractShaderModule.GetStage())),
			m_abstractShaderModule.GetEntryPointName())
	{
	}

	const std::vector<UInt32>& VkRHIShaderModule::GetShaderBytes() const
	{
		return m_abstractShaderModule.GetShaderBytes();
	}

	const std::unordered_map<UInt32, std::vector<cct::gfx::DescriptorSetLayoutBinding>>& VkRHIShaderModule::GetBindings() const
	{
		return m_abstractShaderModule.GetBindings();
	}

	const std::string& VkRHIShaderModule::GetEntryPointName() const
	{
		return m_abstractShaderModule.GetEntryPointName();
	}

	cct::gfx::ShaderStage VkRHIShaderModule::GetStage() const
	{
		return m_abstractShaderModule.GetStage();
	}

	const cct::gfx::ShaderModule& VkRHIShaderModule::GetAbstractShaderModule() const
	{
		return m_abstractShaderModule;
	}

	const vk::ShaderModule& VkRHIShaderModule::GetVulkanShaderModule() const
	{
		return m_vulkanShaderModule;
	}
}
