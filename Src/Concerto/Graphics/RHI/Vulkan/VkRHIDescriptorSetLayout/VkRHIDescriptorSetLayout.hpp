//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSETLAYOUT_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSETLAYOUT_HPP

#include <memory>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSetLayout/DescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/DescriptorSetLayout.hpp"

namespace cct::gfx::rhi
{
	class VkRHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIDescriptorSetLayout : public DescriptorSetLayout, public vk::DescriptorSetLayout
	{
	public:
		VkRHIDescriptorSetLayout(VkRHIDevice& device, std::vector<cct::gfx::DescriptorSetLayoutBinding> bindings);

		const std::vector<cct::gfx::DescriptorSetLayoutBinding>& GetBindings() const override;

	private:
		std::vector<cct::gfx::DescriptorSetLayoutBinding> m_bindings;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSETLAYOUT_HPP
