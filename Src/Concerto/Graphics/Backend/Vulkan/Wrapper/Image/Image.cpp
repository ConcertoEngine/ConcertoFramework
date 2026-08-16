//
// Created by arthur on 17/09/2022.
//

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Image/Image.hpp"

#include <format>

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Allocator/Allocator.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"
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

#ifdef CCT_PLATFORM_WINDOWS
	Image::Image(Device& device, VkExtent2D extent, VkImage image, VkFormat format, VkDeviceMemory externalMemory) :
		Object(device),
		m_isAllocated(false),
		m_imageFormat(format),
		m_extent(extent),
		m_externalMemory(externalMemory)
	{
		m_handle = image;
	}
#endif

	Image::~Image()
	{
		if (!IsValid())
			return;
#ifdef CCT_PLATFORM_WINDOWS
		if (m_externalMemory != VK_NULL_HANDLE)
		{
			GetDevice()->vkFreeMemory(*GetDevice()->Get(), m_externalMemory, nullptr);
			GetDevice()->vkDestroyImage(*GetDevice()->Get(), m_handle, nullptr);
			return;
		}
#endif
		if (!m_isAllocated)
			return;
		vmaDestroyImage(*GetDevice()->GetAllocator().Get(), m_handle, m_allocation);
	}

	Image::Image(Image&& image) noexcept :
		Object(std::move(image)),
		m_isAllocated(std::exchange(image.m_isAllocated, false)),
		m_imageFormat(std::exchange(image.m_imageFormat, {})),
		m_allocation(std::exchange(image.m_allocation, nullptr)),
#ifdef CCT_PLATFORM_WINDOWS
		m_externalMemory(std::exchange(image.m_externalMemory, VK_NULL_HANDLE)),
#endif
		m_extent(std::exchange(image.m_extent, {}))
	{
	}

	Image& Image::operator=(Image&& image) noexcept
	{
		std::swap(m_isAllocated, image.m_isAllocated);
		std::swap(m_imageFormat, image.m_imageFormat);
		std::swap(m_allocation, image.m_allocation);
#ifdef CCT_PLATFORM_WINDOWS
		std::swap(m_externalMemory, image.m_externalMemory);
#endif
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
			1};
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

#ifdef CCT_PLATFORM_WINDOWS
	namespace
	{
		uint32_t FindMemoryType(Device& device, const VkMemoryRequirements& reqs, VkMemoryPropertyFlags props)
		{
			VkPhysicalDeviceMemoryProperties memProps{};
			device.GetPhysicalDevice().GetInstance().vkGetPhysicalDeviceMemoryProperties(
				*device.GetPhysicalDevice().Get(), &memProps);
			for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
			{
				if ((reqs.memoryTypeBits & (1u << i)) &&
					(memProps.memoryTypes[i].propertyFlags & props) == props)
				{
					return i;
				}
			}
			return UINT32_MAX;
		}
	} // namespace

	std::optional<Image> Image::ImportFromWin32Handle(
		Device& device,
		VkExtent2D extent,
		VkFormat format,
		HANDLE ntHandle)
	{
		const VkDevice vkDevice = *device.Get();

		VkExternalMemoryImageCreateInfoKHR extInfo{};
		extInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR;
		extInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT;

		VkExtent3D extent3D = {extent.width, extent.height, 1};
		VkImageCreateInfo imgInfo = VulkanInitializer::ImageCreateInfo(
			format,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			extent3D);
		imgInfo.pNext = &extInfo;

		VkImage vkImage = VK_NULL_HANDLE;
		if (device.vkCreateImage(vkDevice, &imgInfo, nullptr, &vkImage) != VK_SUCCESS)
			return std::nullopt;

		VkImageMemoryRequirementsInfo2KHR reqInfo{};
		reqInfo.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2_KHR;
		reqInfo.image = vkImage;

		VkMemoryDedicatedRequirementsKHR dedicated{};
		dedicated.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS_KHR;

		VkMemoryRequirements2KHR reqs2{};
		reqs2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2_KHR;
		reqs2.pNext = &dedicated;

		device.vkGetImageMemoryRequirements2(vkDevice, &reqInfo, &reqs2);

		const uint32_t memTypeIdx = FindMemoryType(device, reqs2.memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (memTypeIdx == UINT32_MAX)
		{
			device.vkDestroyImage(vkDevice, vkImage, nullptr);
			return std::nullopt;
		}

		VkMemoryDedicatedAllocateInfoKHR dedicatedAlloc{};
		dedicatedAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR;
		dedicatedAlloc.image = vkImage;

		VkImportMemoryWin32HandleInfoKHR importInfo{};
		importInfo.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
		importInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT;
		importInfo.handle = ntHandle;
		if (dedicated.requiresDedicatedAllocation)
			importInfo.pNext = &dedicatedAlloc;

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = &importInfo;
		allocInfo.allocationSize = reqs2.memoryRequirements.size;
		allocInfo.memoryTypeIndex = memTypeIdx;

		VkDeviceMemory memory = VK_NULL_HANDLE;
		if (device.vkAllocateMemory(vkDevice, &allocInfo, nullptr, &memory) != VK_SUCCESS)
		{
			device.vkDestroyImage(vkDevice, vkImage, nullptr);
			return std::nullopt;
		}

		device.vkBindImageMemory(vkDevice, vkImage, memory, 0);

		return Image(device, extent, vkImage, format, memory);
	}
#endif
} // namespace cct::gfx::vk
