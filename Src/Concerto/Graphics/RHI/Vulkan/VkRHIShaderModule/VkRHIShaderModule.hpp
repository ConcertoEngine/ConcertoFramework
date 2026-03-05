//
// Created by arthur on 05/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHISHADERMODULE_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHISHADERMODULE_HPP

#include <string>
#include <vector>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ShaderModule/ShaderModule.hpp"

namespace cct::gfx::vk
{
	class Device;
}

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIShaderModule : public vk::ShaderModule
	{
	public:
		VkRHIShaderModule(vk::Device& device, const std::string& spirvPath, ShaderStageFlags stage,
		                  const std::string& entryPoint = "main");
		VkRHIShaderModule(vk::Device& device, const std::vector<UInt32>& bytes, ShaderStageFlags stage,
		                  const std::string& entryPoint = "main");
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHISHADERMODULE_HPP
