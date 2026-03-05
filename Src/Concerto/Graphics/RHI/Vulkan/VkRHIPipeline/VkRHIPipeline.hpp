//
// Created by arthur on 05/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINE_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINE_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Pipeline/Pipeline.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PipelineInfo/PipelineInfo.hpp"

namespace cct::gfx::vk
{
	class Device;
	class RenderPass;
}

namespace cct::gfx::rhi
{
	class VkRHIRenderPass;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIPipeline : public vk::Pipeline
	{
	public:
		VkRHIPipeline(vk::Device& device, vk::PipelineInfo pipelineInfo, const VkRHIRenderPass& renderPass);
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINE_HPP
