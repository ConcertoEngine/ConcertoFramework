//
// Created by arthur on 05/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORPOOL_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORPOOL_HPP

#include <memory>
#include <span>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorPool/DescriptorPool.hpp"

namespace cct::gfx::vk
{
	class Device;
}

namespace cct::gfx::rhi
{
	class VkRHIDescriptorSetLayout;
	class VkRHIDescriptorSet;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIDescriptorPool : public vk::DescriptorPool
	{
	public:
		VkRHIDescriptorPool(vk::Device& device, std::span<const DescriptorPoolSize> poolSizes);

		std::unique_ptr<VkRHIDescriptorSet> AllocateDescriptorSet(VkRHIDescriptorSetLayout& layout);
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORPOOL_HPP
