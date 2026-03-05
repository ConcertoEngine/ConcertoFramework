//
// Created by arthur on 05/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSETLAYOUT_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSETLAYOUT_HPP

#include <span>
#include <vector>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSetLayout/DescriptorSetLayout.hpp"

namespace cct::gfx::vk
{
	class Device;
}

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIDescriptorSetLayout : public vk::DescriptorSetLayout
	{
	public:
		VkRHIDescriptorSetLayout(vk::Device& device, std::span<const DescriptorBinding> bindings);
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSETLAYOUT_HPP
