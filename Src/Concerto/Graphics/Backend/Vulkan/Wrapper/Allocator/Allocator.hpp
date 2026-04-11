//
// Created by arthu on 21/06/2022.
//

#ifndef CONCERTO_GRAPHICS_ALLOCATOR_HPP
#define CONCERTO_GRAPHICS_ALLOCATOR_HPP

#include <vk_mem_alloc.h>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Buffer/Buffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Image/Image.hpp"

namespace cct::gfx::vk
{
	class Device;
	class Buffer;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API Allocator : public Object<VmaAllocator>
	{
	public:
		Allocator() = default;
		explicit Allocator(Device& device);
		~Allocator() override;

		Allocator(const Allocator&) = delete;
		Allocator(Allocator&&) = default;

		Allocator& operator=(const Allocator&) = delete;
		Allocator& operator=(Allocator&&) = default;

		VkResult Create(Device& device);

		inline Buffer AllocateBuffer(std::size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool allowBufferMapping);
		template<typename T>
		Buffer AllocateBuffer(VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool allowBufferMapping);
		template<typename T>
		Buffer AllocateBuffer(std::size_t objNumber, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool allowBufferMapping);

		inline Image AllocateImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags usageFlags);
	};
}

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Allocator/Allocator.inl"

#endif //CONCERTO_GRAPHICS_ALLOCATOR_HPP
