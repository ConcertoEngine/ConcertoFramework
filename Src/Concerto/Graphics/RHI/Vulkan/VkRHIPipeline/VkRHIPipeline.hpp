//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINE_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINE_HPP

#include <memory>
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Pipeline.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Pipeline/Pipeline.hpp"

namespace cct::gfx::rhi
{
	class VkRHIPipelineLayout;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIPipeline : public Pipeline
	{
	public:
		VkRHIPipeline(std::shared_ptr<vk::Pipeline> pipeline, std::shared_ptr<VkRHIPipelineLayout> pipelineLayout);

		[[nodiscard]] const vk::Pipeline& GetPipeline() const;
		[[nodiscard]] VkPipeline GetVkPipeline() const;
		[[nodiscard]] const VkRHIPipelineLayout& GetLayout() const;
		[[nodiscard]] std::shared_ptr<VkRHIPipelineLayout> GetLayoutPtr() const;

	private:
		std::shared_ptr<vk::Pipeline> m_pipeline;
		std::shared_ptr<VkRHIPipelineLayout> m_pipelineLayout;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIPIPELINE_HPP
