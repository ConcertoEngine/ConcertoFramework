//
// Created by arthur on 15/05/2024.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIRenderPass/VkRHIRenderPass.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"

namespace cct::gfx::rhi
{
	VkRHIRenderPass::VkRHIRenderPass(VkRHIDevice& device, std::span<VkAttachmentDescription> attachmentDescriptions, std::span<VkSubpassDescription> subPassDescriptions, std::span<VkSubpassDependency> subPassDependencies) :
		vk::RenderPass(device, attachmentDescriptions, subPassDescriptions, subPassDependencies)
	{
	}
}
