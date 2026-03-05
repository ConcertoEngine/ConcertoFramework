//
// Created by arthur on 05/03/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorPool/VkRHIDescriptorPool.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSetLayout/VkRHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSet/VkRHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::rhi
{
	VkRHIDescriptorPool::VkRHIDescriptorPool(vk::Device& device, std::span<const DescriptorPoolSize> poolSizes)
		: vk::DescriptorPool(device, [&]() {
			std::vector<VkDescriptorPoolSize> vkSizes;
			vkSizes.reserve(poolSizes.size());
			for (const auto& s : poolSizes)
				vkSizes.push_back({ Converters::ToVulkan(s.type), s.count });
			return vkSizes;
		}())
	{
	}

	std::unique_ptr<VkRHIDescriptorSet> VkRHIDescriptorPool::AllocateDescriptorSet(VkRHIDescriptorSetLayout& layout)
	{
		return std::make_unique<VkRHIDescriptorSet>(*this, layout);
	}
}
