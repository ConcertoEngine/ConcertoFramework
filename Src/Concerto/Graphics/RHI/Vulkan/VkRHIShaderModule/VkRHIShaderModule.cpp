//
// Created by arthur on 05/03/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIShaderModule/VkRHIShaderModule.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::rhi
{
	VkRHIShaderModule::VkRHIShaderModule(vk::Device& device, const std::string& spirvPath, ShaderStageFlags stage,
	                                     const std::string& entryPoint)
		: vk::ShaderModule(device, spirvPath,
		                   static_cast<VkShaderStageFlagBits>(Converters::ToVulkan(stage)),
		                   entryPoint)
	{
	}

	VkRHIShaderModule::VkRHIShaderModule(vk::Device& device, const std::vector<UInt32>& bytes, ShaderStageFlags stage,
	                                     const std::string& entryPoint)
		: vk::ShaderModule(device, bytes,
		                   static_cast<VkShaderStageFlagBits>(Converters::ToVulkan(stage)),
		                   entryPoint)
	{
	}
}
