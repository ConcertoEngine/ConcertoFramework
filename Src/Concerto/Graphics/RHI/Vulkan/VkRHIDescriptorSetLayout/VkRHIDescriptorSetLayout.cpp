//
// Created by arthur on 05/03/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSetLayout/VkRHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::rhi
{
	VkRHIDescriptorSetLayout::VkRHIDescriptorSetLayout(vk::Device& device, std::span<const DescriptorBinding> bindings)
		: vk::DescriptorSetLayout(device, [&]() {
			std::vector<VkDescriptorSetLayoutBinding> vkBindings;
			vkBindings.reserve(bindings.size());
			for (const auto& b : bindings)
			{
				VkDescriptorSetLayoutBinding vkb{};
				vkb.binding         = b.binding;
				vkb.descriptorType  = Converters::ToVulkan(b.type);
				vkb.descriptorCount = b.count;
				vkb.stageFlags      = Converters::ToVulkan(b.stageFlags);
				vkBindings.push_back(vkb);
			}
			return vkBindings;
		}())
	{
	}
}
