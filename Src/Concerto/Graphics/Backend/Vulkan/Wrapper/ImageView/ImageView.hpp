//
// Created by arthur on 18/09/2022.
//

#ifndef CONCERTO_GRAPHICS_IMAGEVIEW_HPP
#define CONCERTO_GRAPHICS_IMAGEVIEW_HPP



#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;
	class Image;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API ImageView : public Object<VkImageView>
	{
	 public:
		ImageView();
		ImageView(Device& device, Image& image, VkImageAspectFlags aspectFlags);
		~ImageView() override;

		ImageView(const ImageView&) = delete;
		ImageView(ImageView&&) noexcept = default;

		ImageView& operator=(const ImageView&) = delete;
		ImageView& operator=(ImageView&&) noexcept = default;

		VkResult Create(Device& device, Image& image, VkImageAspectFlags aspectFlags);

		Image& GetImage() const;
	private:
		Image* m_image;
	};
}

#endif //CONCERTO_GRAPHICS_IMAGEVIEW_HPP
