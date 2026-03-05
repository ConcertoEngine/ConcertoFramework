//
// Created by arthur on 05/03/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipelineLayout/VkRHIPipelineLayout.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSetLayout/VkRHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::rhi
{
	VkRHIPipelineLayout::VkRHIPipelineLayout(vk::Device& device, std::span<VkRHIDescriptorSetLayout*> layouts)
		: vk::PipelineLayout(device, [&]() {
			std::vector<std::shared_ptr<vk::DescriptorSetLayout>> vkLayouts;
			vkLayouts.reserve(layouts.size());
			for (auto* l : layouts)
				vkLayouts.push_back(std::shared_ptr<vk::DescriptorSetLayout>(l, [](auto*) {})); // non-owning
			return vkLayouts;
		}())
	{
	}
}
