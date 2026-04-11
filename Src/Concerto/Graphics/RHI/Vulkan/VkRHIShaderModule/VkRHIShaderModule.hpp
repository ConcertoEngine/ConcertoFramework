//
// Created by arthur on 23/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHISHADERMODULE_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHISHADERMODULE_HPP

#include <memory>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/ShaderModule.hpp"
#include "Concerto/Graphics/Core/ShaderModule/ShaderModule.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ShaderModule/ShaderModule.hpp"

namespace cct::gfx::vk
{
	class Device;
}

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIShaderModule : public ShaderModule
	{
	public:
		VkRHIShaderModule() = delete;
		VkRHIShaderModule(vk::Device& device, cct::gfx::ShaderModule&& shaderModule);
		VkRHIShaderModule(const VkRHIShaderModule&) = delete;
		VkRHIShaderModule(VkRHIShaderModule&&) = default;

		VkRHIShaderModule& operator=(const VkRHIShaderModule&) = delete;
		VkRHIShaderModule& operator=(VkRHIShaderModule&&) = default;

		// RHI ShaderModule interface implementation
		const std::vector<UInt32>& GetShaderBytes() const override;
		const std::unordered_map<UInt32, std::vector<cct::gfx::DescriptorSetLayoutBinding>>& GetBindings() const override;
		const std::string& GetEntryPointName() const override;
		cct::gfx::ShaderStage GetStage() const override;

		// Vulkan-specific methods
		const cct::gfx::ShaderModule& GetAbstractShaderModule() const;
		const vk::ShaderModule& GetVulkanShaderModule() const;

	private:
		cct::gfx::ShaderModule m_abstractShaderModule;
		vk::ShaderModule m_vulkanShaderModule;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHISHADERMODULE_HPP
