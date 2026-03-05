//
// Created by arthur on 05/03/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipeline/VkRHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIRenderPass/VkRHIRenderPass.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::rhi
{
	VkRHIPipeline::VkRHIPipeline(vk::Device& device, vk::PipelineInfo pipelineInfo, const VkRHIRenderPass& renderPass)
		: vk::Pipeline(device, std::move(pipelineInfo), renderPass)
	{
	}
}
