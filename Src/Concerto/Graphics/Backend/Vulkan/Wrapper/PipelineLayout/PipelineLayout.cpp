//
// Created by arthur on 30/06/2022.
//

#include <stdexcept>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSetLayout/DescriptorSetLayout.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PipelineLayout/PipelineLayout.hpp"

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{

	PipelineLayout::PipelineLayout(Device& device, const std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts) :
		Object(device),
		m_descriptorSetLayouts(descriptorSetLayouts)
	{
		if (Create(device, descriptorSetLayouts) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	PipelineLayout::~PipelineLayout()
	{
		if (!IsValid())
			return;
		m_device->vkDestroyPipelineLayout(*m_device->Get(), m_handle, nullptr);
	}

	VkResult PipelineLayout::Create(Device& device, const std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts)
	{
		m_device = &device;
		m_descriptorSetLayouts = descriptorSetLayouts;

		std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts;
		vkDescriptorSetLayouts.reserve(m_descriptorSetLayouts.size());
		for (const auto& descriptorSetLayout : m_descriptorSetLayouts)
			vkDescriptorSetLayouts.push_back(*descriptorSetLayout->Get());

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<UInt32>(vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		m_lastResult = m_device->vkCreatePipelineLayout(*m_device->Get(), &pipelineLayoutCreateInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreatePipelineLayout failed VKResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}

	const std::vector<std::shared_ptr<DescriptorSetLayout>>& PipelineLayout::GetDescriptorSetLayouts() const
	{
		return m_descriptorSetLayouts;
	}
}
