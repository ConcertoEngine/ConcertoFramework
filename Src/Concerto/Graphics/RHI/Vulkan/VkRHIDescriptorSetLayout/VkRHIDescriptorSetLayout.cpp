//
// Created by arthur on 24/10/2025.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSetLayout/VkRHIDescriptorSetLayout.hpp"

#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"

namespace cct::gfx::rhi
{
	VkRHIDescriptorSetLayout::VkRHIDescriptorSetLayout(VkRHIDevice& device, std::vector<cct::gfx::DescriptorSetLayoutBinding> bindings) :
		rhi::DescriptorSetLayout(),
		vk::DescriptorSetLayout(),
		m_bindings(std::move(bindings))
	{
		std::vector<VkDescriptorSetLayoutBinding> vkBindings;
		vkBindings.reserve(m_bindings.size());

		for (const auto& binding : m_bindings)
		{
			auto& b = vkBindings.emplace_back();
			b.binding = binding.binding;
			b.descriptorType = Converters::ToVulkan(binding.descriptorType);
			b.descriptorCount = binding.descriptorCount;
			b.stageFlags = Converters::ToVulkan(binding.stageFlags);
			b.pImmutableSamplers = nullptr;
			
		}

		vk::DescriptorSetLayout::Create(device, vkBindings);
	}

	const std::vector<cct::gfx::DescriptorSetLayoutBinding>& VkRHIDescriptorSetLayout::GetBindings() const
	{
		return m_bindings;
	}
}
