//
// Created by arthur on 18/08/22.
//

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PipelineInfo/PipelineInfo.hpp"

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Vertex/Vertex.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"

namespace cct::gfx::vk
{
	PipelineInfo::PipelineInfo() :
		m_vertexDescription(),
		m_shaderStages(),
		m_vertexInputInfo(),
		m_inputAssembly(),
		m_viewport(),
		m_scissor(),
		m_rasterizer(),
		m_colorBlendAttachment(),
		m_multisampling(),
		m_pipelineLayout(nullptr),
		m_depthStencil()
	{
	}

	PipelineInfo::PipelineInfo(std::vector<VkPipelineShaderStageCreateInfo> shaderStages, VkExtent2D windowExtent, const PipelineLayout& pipelineLayout)
	{
		m_vertexDescription = GetVertexDescription();
		m_shaderStages = std::move(shaderStages);
		m_vertexInputInfo = VulkanInitializer::VertexInputStateCreateInfo();
		m_vertexInputInfo.pVertexAttributeDescriptions = m_vertexDescription.attributes.data();
		m_vertexInputInfo.vertexAttributeDescriptionCount = static_cast<UInt32>(m_vertexDescription.attributes.size());
		m_vertexInputInfo.pVertexBindingDescriptions = m_vertexDescription.bindings.data();
		m_vertexInputInfo.vertexBindingDescriptionCount = static_cast<UInt32>(m_vertexDescription.bindings.size());
		m_inputAssembly = VulkanInitializer::InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		m_viewport.x = 0.0f;
		m_viewport.y = 0.0f;
		m_viewport.width = static_cast<float>(windowExtent.width);
		m_viewport.height = static_cast<float>(windowExtent.height);
		m_viewport.minDepth = 0.0f;
		m_viewport.maxDepth = 1.0f;
		m_scissor.offset = {0, 0};
		m_scissor.extent = windowExtent;
		m_rasterizer = VulkanInitializer::RasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
		m_multisampling = VulkanInitializer::MultisamplingStateCreateInfo();
		m_colorBlendAttachment = VulkanInitializer::ColorBlendAttachmentState();
		m_pipelineLayout = &pipelineLayout;
		m_depthStencil = VulkanInitializer::DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
	}
} // namespace cct::gfx::vk