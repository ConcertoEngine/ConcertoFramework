//
// Created by arthu on 24/08/2025.
//

#ifndef CONCERTO_GRAPHICS_ALLOCATOR_INL
#define CONCERTO_GRAPHICS_ALLOCATOR_INL

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Allocator/Allocator.hpp"

namespace cct::gfx::vk
{
	Buffer Allocator::AllocateBuffer(std::size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
	                                 bool allowBufferMapping)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		return { *this, allocSize, usage, memoryUsage, allowBufferMapping };
	}

	template <typename T>
	Buffer Allocator::AllocateBuffer(VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool allowBufferMapping)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		return { *this, sizeof(T), usage, memoryUsage, allowBufferMapping };
	}

	template <typename T>
	Buffer Allocator::AllocateBuffer(std::size_t objNumber, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
		bool allowBufferMapping)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		return { *this, sizeof(T) * objNumber, usage, memoryUsage, allowBufferMapping };
	}

	inline Image Allocator::AllocateImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags usageFlags)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		return { *this, extent, format, usageFlags };
	}
}

#endif //CONCERTO_GRAPHICS_ALLOCATOR_INL
