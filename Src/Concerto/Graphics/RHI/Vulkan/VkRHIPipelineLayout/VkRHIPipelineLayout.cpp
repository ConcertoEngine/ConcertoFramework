//
// Created by arthur on 24/10/2025.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipelineLayout/VkRHIPipelineLayout.hpp"

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSetLayout/VkRHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"

namespace cct::gfx::rhi
{
	VkRHIPipelineLayout::VkRHIPipelineLayout(VkRHIDevice& device, std::span<const std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts) :
		rhi::PipelineLayout(),
		vk::PipelineLayout()
	{
		std::vector<std::shared_ptr<vk::DescriptorSetLayout>> vkDescriptorSetLayouts;
		vkDescriptorSetLayouts.reserve(descriptorSetLayouts.size());

		for (const auto& descriptorSetLayout : descriptorSetLayouts)
			vkDescriptorSetLayouts.push_back(std::static_pointer_cast<VkRHIDescriptorSetLayout>(descriptorSetLayout));

		vk::PipelineLayout::Create(device, vkDescriptorSetLayouts);
		m_descriptorSetLayouts.assign(descriptorSetLayouts.begin(), descriptorSetLayouts.end());
	}

	const std::vector<std::shared_ptr<DescriptorSetLayout>>& VkRHIPipelineLayout::GetDescriptorSetLayouts() const
	{
		return m_descriptorSetLayouts;
	}
}
