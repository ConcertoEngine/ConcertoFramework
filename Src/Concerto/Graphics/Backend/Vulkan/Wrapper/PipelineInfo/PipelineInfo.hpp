//
// Created by arthur on 10/06/22.
//

#ifndef CONCERTO_GRAPHICS_PIPELINEINFO_HPP
#define CONCERTO_GRAPHICS_PIPELINEINFO_HPP

#include <vector>
#include <memory>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PipelineLayout/PipelineLayout.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Vertex/Vertex.hpp"

namespace cct::gfx::vk
{

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API PipelineInfo
	{
	public:
		PipelineInfo();
		PipelineInfo(std::vector<VkPipelineShaderStageCreateInfo> shaderStages, VkExtent2D windowExtent, const PipelineLayout& pipelineLayout);

		VertexInputDescription m_vertexDescription;
		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
		VkPipelineVertexInputStateCreateInfo m_vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo m_inputAssembly;
		VkViewport m_viewport;
		VkRect2D m_scissor;
		VkPipelineRasterizationStateCreateInfo m_rasterizer;
		VkPipelineColorBlendAttachmentState m_colorBlendAttachment;
		VkPipelineMultisampleStateCreateInfo m_multisampling;
		const PipelineLayout* m_pipelineLayout;
		VkPipelineDepthStencilStateCreateInfo m_depthStencil;
	};

} // cct::gfx::vk

#endif //CONCERTO_GRAPHICS_PIPELINEINFO_HPP