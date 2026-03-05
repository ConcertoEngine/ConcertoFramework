//
// Created by arthur on 05/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINELAYOUT_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINELAYOUT_HPP

#include <memory>
#include <span>
#include <vector>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PipelineLayout/PipelineLayout.hpp"

namespace cct::gfx::vk
{
	class Device;
}

namespace cct::gfx::rhi
{
	class VkRHIDescriptorSetLayout;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIPipelineLayout : public vk::PipelineLayout
	{
	public:
		VkRHIPipelineLayout(vk::Device& device, std::span<VkRHIDescriptorSetLayout*> layouts);
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINELAYOUT_HPP
