//
// Created by arthur on 05/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSET_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSET_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSet/DescriptorSet.hpp"

namespace cct::gfx::rhi
{
	class VkRHISampler;
	class VkRHITexture;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIDescriptorSet : public vk::DescriptorSet
	{
	public:
		using vk::DescriptorSet::DescriptorSet;

		void WriteImageSamplerDescriptor(const VkRHISampler& sampler, const VkRHITexture& texture,
		                                 ImageLayout layout = ImageLayout::ShaderReadOnlyOptimal);
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSET_HPP
