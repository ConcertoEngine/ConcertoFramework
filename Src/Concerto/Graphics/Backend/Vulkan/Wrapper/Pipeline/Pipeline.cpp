//
// Created by arthur on 10/06/22.
//

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Pipeline/Pipeline.hpp"

#include <iostream>
#include <utility>

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PipelineLayout/PipelineLayout.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/RenderPass/RenderPass.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"

namespace cct::gfx::vk
{

	Pipeline::Pipeline(Device& device, PipelineInfo pipeLineInfo, const RenderPass& renderPass) :
		Object(device),
		m_pipelineInfo(std::move(pipeLineInfo)),
		m_createInfo(),
		m_renderPass(&renderPass)
	{
		if (Create(device, m_pipelineInfo, renderPass) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	Pipeline::Pipeline(Device& device, const VkPipelineShaderStageCreateInfo& computeStage, VkPipelineLayout layout) :
		Object(device),
		m_createInfo(),
		m_renderPass(nullptr)
	{
		VkComputePipelineCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		info.stage = computeStage;
		info.layout = layout;
		m_lastResult = m_device->vkCreateComputePipelines(*m_device->Get(), VK_NULL_HANDLE, 1, &info, nullptr, &m_handle);
		if (m_lastResult != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	Pipeline::~Pipeline()
	{
		if (!IsValid())
			return;
		m_device->vkDestroyPipeline(*m_device->Get(), m_handle, nullptr);
	}

	VkResult Pipeline::Create(Device& device, const PipelineInfo& pipeLineInfo, const RenderPass& renderPass)
	{
		m_device = &device;
		m_pipelineInfo = pipeLineInfo;
		m_renderPass = &renderPass;

		m_createInfo.viewportState = BuildViewportState();
		m_createInfo.colorBlend = BuildColorBlendState();
		m_createInfo.pipelineCreateInfo = {};
		m_createInfo.pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		return BuildPipeline();
	}

	VkPipelineViewportStateCreateInfo Pipeline::BuildViewportState()
	{
		VkPipelineViewportStateCreateInfo viewportState{};
		static VkRect2D scissor = {0, 0}; // TODO: remove static
		scissor.offset = {.x = 0, .y = 0};
		scissor.extent = {.width = 1280, .height = 720};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;
		viewportState.viewportCount = 1;
		//		viewportState.pViewports = &m_pipelineInfo.m_viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;
		return viewportState;
	}

	VkPipelineColorBlendStateCreateInfo Pipeline::BuildColorBlendState()
	{
		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		static VkPipelineColorBlendAttachmentState colorBlendAttachment(
			VulkanInitializer::ColorBlendAttachmentState()); // TODO: remove static
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext = nullptr;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		return colorBlending;
	}

	const PipelineLayout& Pipeline::GetPipelineLayout() const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		return *m_pipelineInfo.m_pipelineLayout;
	}

	VkResult Pipeline::BuildPipeline()
	{
		CCT_AUTO_PROFILER_SCOPE();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		VkPipelineViewportStateCreateInfo viewportState{};

		VkDynamicState dynamicState[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = 2;
		dynamicStateCreateInfo.pDynamicStates = dynamicState;

		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext = nullptr;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &m_pipelineInfo.m_colorBlendAttachment;

		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = nullptr;

		pipelineInfo.pStages = m_pipelineInfo.m_shaderStages.data();
		pipelineInfo.stageCount = static_cast<UInt32>(m_pipelineInfo.m_shaderStages.size());
		pipelineInfo.pVertexInputState = &m_pipelineInfo.m_vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &m_pipelineInfo.m_inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineInfo.pRasterizationState = &m_pipelineInfo.m_rasterizer;
		pipelineInfo.pMultisampleState = &m_pipelineInfo.m_multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = *m_pipelineInfo.m_pipelineLayout->Get();
		pipelineInfo.renderPass = *m_renderPass->Get();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.pDepthStencilState = &m_pipelineInfo.m_depthStencil;

		m_lastResult = m_device->vkCreateGraphicsPipelines(*m_device->Get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateGraphicsPipelines failed VKResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}

} // namespace cct::gfx::vk
