//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHITEXTURE_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHITEXTURE_HPP

#include <memory>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Image/Image.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ImageView/ImageView.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"

namespace cct::gfx::vk
{
	class Device;
	class CommandBuffer;
	class UploadContext;
	class Queue;
} // namespace cct::gfx::vk

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHITexture : public rhi::Texture
	{
	public:
		VkRHITexture(vk::Device& device, PixelFormat format, Int32 width, Int32 height, VkImageAspectFlags aspectFlags);
		VkRHITexture(vk::Device& device, vk::Image image, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		std::unique_ptr<rhi::TextureView> CreateView() const override;
		const vk::Image& GetImage() const;
		const vk::ImageView& GetImageView() const;

	private:
		vk::Image m_image;
		vk::ImageView m_imageView;
	};
	using VkRHITexturePtr = std::shared_ptr<VkRHITexture>;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHITextureView : public rhi::TextureView
	{
	public:
		VkRHITextureView(const vk::ImageView& image);
		const vk::ImageView& GetImageView() const;

	private:
		const vk::ImageView* m_imageView;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_VULKAN_VKRHITEXTURE_HPP