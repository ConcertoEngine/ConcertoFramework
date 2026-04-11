//
// Created by arthur on 18/09/2022.
//

#include <utility>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ImageView/ImageView.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Image/Image.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"

namespace cct::gfx::vk
{
	ImageView::ImageView() :
		m_image(nullptr)
	{
	}

	ImageView::ImageView(Device& device, Image& image, VkImageAspectFlags aspectFlags) :
		Object(device),
		m_image(&image)
	{
		if (Create(device, image, aspectFlags) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	ImageView::~ImageView()
	{
		if (!IsValid())
			return;
		m_device->WaitIdle();
		m_device->vkDestroyImageView(*m_device->Get(), m_handle, nullptr);
	}

	VkResult ImageView::Create(Device& device, Image& image, VkImageAspectFlags aspectFlags)
	{
		m_device = &device;
		m_image = &image;

		auto imageInfo = VulkanInitializer::ImageViewCreateInfo(image.GetFormat(), *image.Get(), aspectFlags);

		m_lastResult = m_device->vkCreateImageView(*m_device->Get(), &imageInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateImageView failed VkResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}

	Image& ImageView::GetImage() const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		return *m_image;
	}
}
