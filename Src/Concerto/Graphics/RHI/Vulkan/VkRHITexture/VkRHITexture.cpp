//
// Created by arthur on 17/08/2022.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"

#include "Concerto/Graphics/Backend/Vulkan/UploadContext/UploadContext.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"

namespace cct::gfx::rhi
{
	VkRHITexture::VkRHITexture(vk::Device& device, PixelFormat format, Int32 width, Int32 height, VkImageAspectFlags aspectFlags) :
		m_image(device.GetAllocator().AllocateImage(
			VkExtent2D{static_cast<UInt32>(width), static_cast<UInt32>(height)},
			Converters::ToVulkan(format),
			(aspectFlags & VK_IMAGE_ASPECT_DEPTH_BIT)
				? (VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
				: (VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT))),
		m_imageView(device, m_image, aspectFlags)
	{
	}

	VkRHITexture::VkRHITexture(vk::Device& device, vk::Image image, VkImageAspectFlags aspectFlags) :
		m_image(std::move(image)),
		m_imageView(device, m_image, aspectFlags)
	{
	}

	std::unique_ptr<rhi::TextureView> VkRHITexture::CreateView() const
	{
		return std::make_unique<VkRHITextureView>(m_imageView);
	}

	const vk::Image& VkRHITexture::GetImage() const
	{
		return m_image;
	}

	const vk::ImageView& VkRHITexture::GetImageView() const
	{
		return m_imageView;
	}

	VkRHITextureView::VkRHITextureView(const vk::ImageView& image) :
		m_imageView(&image)
	{
	}

	const vk::ImageView& VkRHITextureView::GetImageView() const
	{
		return *m_imageView;
	}
} // namespace cct::gfx::rhi
