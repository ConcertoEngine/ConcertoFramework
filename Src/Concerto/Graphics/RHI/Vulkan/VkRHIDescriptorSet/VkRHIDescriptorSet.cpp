//
// Created by arthur on 05/03/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSet/VkRHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHISampler/VkRHISampler.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"

namespace cct::gfx::rhi
{
	void VkRHIDescriptorSet::WriteImageSamplerDescriptor(const VkRHISampler& sampler, const VkRHITexture& texture, ImageLayout layout)
	{
		vk::DescriptorSet::WriteImageSamplerDescriptor(sampler, texture.GetImageView(), Converters::ToVulkan(layout));
	}
}
