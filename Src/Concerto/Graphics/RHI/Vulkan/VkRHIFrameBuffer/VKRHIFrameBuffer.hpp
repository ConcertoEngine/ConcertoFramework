//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_FRAMEBUFFER_HPP
#define CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_FRAMEBUFFER_HPP

#include <memory>

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"
#include "Concerto/Graphics/RHI/FrameBuffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/FrameBuffer/FrameBuffer.hpp"

namespace cct::gfx::rhi
{
	class VkRHIDevice;
	class VkRHIRenderPass;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIFrameBuffer final : public FrameBuffer, public vk::FrameBuffer
	{
	public:
		VkRHIFrameBuffer(VkRHIDevice& device, UInt32 width, UInt32 height, const VkRHIRenderPass& renderPass, const std::vector<std::unique_ptr<rhi::Texture>>& attachments);
		VkRHIFrameBuffer(VkRHIDevice& device, UInt32 width, UInt32 height, const VkRHIRenderPass& renderPass, const std::vector<std::unique_ptr<rhi::TextureView>>& attachments);
		UInt32 GetWidth() const override;
		UInt32 GetHeight() const override;
	};
}

#endif //CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_FRAMEBUFFER_HPP