//
// Created by arthur on 09/06/22.
//

#ifndef CONCERTO_GRAPHICS_IMAGE_HPP
#define CONCERTO_GRAPHICS_IMAGE_HPP

#include <string>
#include <optional>

#include <vk_mem_alloc.h>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Allocator;
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API Image : public Object<VkImage>
	{
	public:
		Image();
		/**
		 * @brief Create an Image from an existing VkImage eg: from SwapChain
		 */
		Image(const Allocator& allocator, VkExtent2D extent, VkImage image, VkFormat imageFormat);
		Image(const Allocator& allocator, VkExtent2D extent, VkFormat format, VkImageUsageFlags usageFlags);

#ifdef CCT_PLATFORM_WINDOWS
		/**
		 * @brief Create an Image by importing a D3D11 NT handle (bypasses VMA).
		 *        The caller owns the VkDeviceMemory lifecycle via this Image.
		 */
		Image(Device& device, VkExtent2D extent, VkImage image, VkFormat format, VkDeviceMemory externalMemory);

		/**
		 * @brief Import a D3D11 NTHANDLE-shared texture as a Vulkan external-memory image.
		 *        Returns nullopt on failure (extension unsupported, allocation failed, etc.).
		 */
		static std::optional<Image> ImportFromWin32Handle(
			Device& device,
			VkExtent2D extent,
			VkFormat format,
			HANDLE ntHandle);
#endif

		~Image() override;

		Image(Image&&) noexcept;
		Image(const Image&) = delete;

		Image& operator=(Image&&) noexcept;
		Image& operator=(const Image&) = delete;

		VkResult Create(const Allocator& allocator, VkExtent2D extent, VkImage image, VkFormat imageFormat);
		VkResult Create(const Allocator& allocator, VkExtent2D extent, VkFormat format, VkImageUsageFlags usageFlags);

		[[nodiscard]] VkFormat GetFormat() const;
		VkExtent2D GetExtent() const;
	private:
		bool m_isAllocated;
		VkFormat m_imageFormat = {};
		VmaAllocation m_allocation = {};
		VkExtent2D m_extent;
#ifdef CCT_PLATFORM_WINDOWS
		VkDeviceMemory m_externalMemory = VK_NULL_HANDLE;
#endif
	};
}

#endif //CONCERTO_GRAPHICS_IMAGE_HPP