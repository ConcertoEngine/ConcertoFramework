//
// Created by arthur on 12/06/22.
//


#include <stdexcept>
#include <cassert>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/RenderPass/RenderPass.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{
	RenderPass::RenderPass(Device& device, std::span<VkAttachmentDescription> attachmentDescriptions, std::span<VkSubpassDescription> subPassDescriptions, std::span<VkSubpassDependency> subPassDependencies) :
		Object(device)
	{
		if (Create(device, attachmentDescriptions, subPassDescriptions, subPassDependencies) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	RenderPass::~RenderPass()
	{
		if (!IsValid())
			return;
		m_device->vkDestroyRenderPass(*m_device->Get(), m_handle, nullptr);
	}

	VkResult RenderPass::Create(Device& device, std::span<VkAttachmentDescription> attachmentDescriptions,
		std::span<VkSubpassDescription> subPassDescriptions, std::span<VkSubpassDependency> subPassDependencies)
	{
		m_device = &device;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<UInt32>(attachmentDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
		renderPassCreateInfo.subpassCount = static_cast<UInt32>(subPassDescriptions.size());
		renderPassCreateInfo.pSubpasses = subPassDescriptions.data();
		renderPassCreateInfo.dependencyCount = static_cast<UInt32>(subPassDependencies.size());
		renderPassCreateInfo.pDependencies = subPassDependencies.data();

		m_lastResult = m_device->vkCreateRenderPass(*m_device->Get(), &renderPassCreateInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateRenderPass failed VKResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}
} // cct::gfx::vk
