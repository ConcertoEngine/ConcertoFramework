//
// Created by arthur on 24/10/2025.
//

#include <utility>

#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipeline/VkRHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipelineLayout/VkRHIPipelineLayout.hpp"

namespace cct::gfx::rhi
{
	VkRHIPipeline::VkRHIPipeline(std::shared_ptr<vk::Pipeline> pipeline, std::shared_ptr<VkRHIPipelineLayout> pipelineLayout) :
		rhi::Pipeline(),
		m_pipeline(std::move(pipeline)),
		m_pipelineLayout(std::move(pipelineLayout))
	{
		CCT_ASSERT(m_pipeline && m_pipeline->IsValid(), "Invalid Vulkan pipeline");
		CCT_ASSERT(m_pipelineLayout && m_pipelineLayout->IsValid(), "Invalid pipeline layout");
	}

	const vk::Pipeline& VkRHIPipeline::GetPipeline() const
	{
		return *m_pipeline;
	}

	VkPipeline VkRHIPipeline::GetVkPipeline() const
	{
		return *m_pipeline->Get();
	}

	const VkRHIPipelineLayout& VkRHIPipeline::GetLayout() const
	{
		return *m_pipelineLayout;
	}

	std::shared_ptr<VkRHIPipelineLayout> VkRHIPipeline::GetLayoutPtr() const
	{
		return m_pipelineLayout;
	}
}
