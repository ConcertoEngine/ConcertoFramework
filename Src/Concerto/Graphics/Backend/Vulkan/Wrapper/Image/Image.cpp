//
// Created by arthur on 17/09/2022.
//

#include <format>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Allocator/Allocator.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Image/Image.hpp"

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"

namespace cct::gfx::vk
{
	Image::Image() :
		m_isAllocated(false),
		m_extent()
	{
	}

	Image::Image(const Allocator& allocator, VkExtent2D extent, VkFormat format, VkImageUsageFlags usageFlags) :
		Object(*allocator.GetDevice()),
		m_isAllocated(true),
		m_imageFormat(format),
		m_extent(extent)
	{
		if (Create(allocator, extent, format, usageFlags) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	Image::Image(const Allocator& allocator, VkExtent2D extent, VkImage image, VkFormat imageFormat) :
		Object(*allocator.GetDevice()),
		m_isAllocated(false),
		m_imageFormat(imageFormat),
		m_extent(extent)
	{
		if (Create(allocator, extent, image, imageFormat) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	Image::~Image()
	{
		if (!IsValid())
			return;
		if (!m_isAllocated)
			return;
		vmaDestroyImage(*GetDevice()->GetAllocator().Get(), m_handle, m_allocation);
	}

	Image::Image(Image&& image) noexcept :
		Object(std::move(image)),
		m_isAllocated(std::exchange(image.m_isAllocated, false)),
		m_imageFormat(std::exchange(image.m_imageFormat, {})),
		m_allocation(std::exchange(image.m_allocation, nullptr)),
		m_extent(std::exchange(image.m_extent, {}))
	{
	}

	Image& Image::operator=(Image&& image) noexcept
	{
		std::swap(m_isAllocated, image.m_isAllocated);
		std::swap(m_imageFormat, image.m_imageFormat);
		std::swap(m_allocation, image.m_allocation);
		std::swap(m_extent, image.m_extent);
		Object::operator=(std::move(image));
		return *this;
	}

	VkResult Image::Create(const Allocator& allocator, VkExtent2D extent, VkImage image, VkFormat imageFormat)
	{
		m_device = allocator.GetDevice();
		m_handle = image;

		m_isAllocated = false;
		m_imageFormat = imageFormat,
		m_extent = extent;

		return VK_SUCCESS;
	}

	VkResult Image::Create(const Allocator& allocator, VkExtent2D extent, VkFormat format, VkImageUsageFlags usageFlags)
	{
		VkExtent3D depthImageExtent = {
				extent.width,
				extent.height,
				1
		};
		VkImageCreateInfo imageCreateInfo = VulkanInitializer::ImageCreateInfo(format, usageFlags, depthImageExtent);
		VmaAllocationCreateInfo imageAllocInfo = {};
		imageAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		imageAllocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_lastResult = vmaCreateImage(*allocator.Get(), &imageCreateInfo, &imageAllocInfo, &m_handle, &m_allocation, nullptr);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vmaCreateImage failed VkResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}


	VkFormat Image::GetFormat() const
	{
		return m_imageFormat;
	}

	VkExtent2D Image::GetExtent() const
	{
		return m_extent;
	}
}
