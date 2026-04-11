//
// Created by arthur on 12/06/22.
//

#ifndef CONCERTO_GRAPHICS_FRAMEBUFFER_HPP
#define CONCERTO_GRAPHICS_FRAMEBUFFER_HPP

#include <vector>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;
	class RenderPass;
	class ImageView;


	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API FrameBuffer : public Object<VkFramebuffer>
	{
	public:
		FrameBuffer();
		FrameBuffer(const RenderPass& renderPass, const std::vector<VkImageView>& attachments, VkExtent2D extent);
		~FrameBuffer() override;

		FrameBuffer(FrameBuffer&&) noexcept = default;
		FrameBuffer(const FrameBuffer&) = delete;

		FrameBuffer& operator=(FrameBuffer&&) noexcept = default;
		FrameBuffer& operator=(const FrameBuffer&) = delete;

		VkResult Create(const RenderPass& renderPass, const std::vector<VkImageView>& attachments, VkExtent2D extent);

		VkExtent2D GetExtent2D() const;
	private:
		VkExtent2D m_extent;
	};

} // cct::gfx::vk

#endif //CONCERTO_GRAPHICS_FRAMEBUFFER_HPP
