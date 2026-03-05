//
// Created by arthur on 17/08/2022.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/UploadContext/UploadContext.hpp"

namespace cct::gfx::rhi
{
	VkRHITexture::VkRHITexture(vk::Device& device, PixelFormat format, Int32 width, Int32 height, VkImageAspectFlags aspectFlags, TextureUsageFlags usage) :
		m_image(device.GetAllocator().AllocateImage(VkExtent2D{ static_cast<UInt32>(width), static_cast<UInt32>(height)}, Converters::ToVulkan(format), Converters::ToVulkan(usage))),
		m_imageView(device, m_image, aspectFlags)
	{
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
}
