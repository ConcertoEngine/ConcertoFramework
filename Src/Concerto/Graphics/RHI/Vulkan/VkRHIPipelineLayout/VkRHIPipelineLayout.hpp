//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINELAYOUT_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINELAYOUT_HPP

#include <memory>
#include <span>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PipelineLayout/PipelineLayout.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/PipelineLayout.hpp"

namespace cct::gfx::rhi
{
	class VkRHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIPipelineLayout : public PipelineLayout, public vk::PipelineLayout
	{
	public:
		VkRHIPipelineLayout(VkRHIDevice& device, std::span<const std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts);

		const std::vector<std::shared_ptr<DescriptorSetLayout>>& GetDescriptorSetLayouts() const override;

	private:
		std::vector<std::shared_ptr<DescriptorSetLayout>> m_descriptorSetLayouts;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINELAYOUT_HPP
