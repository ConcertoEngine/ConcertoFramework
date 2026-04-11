//
// Created by arthur on 21/06/2022.
//

#include <stdexcept>
#include <utility>
#include <vk_mem_alloc.h>

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Buffer/Buffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Allocator/Allocator.hpp"

namespace cct::gfx::vk
{
	Buffer::Buffer() :
		m_allocatedSize(0),
		m_allocator(nullptr),
		m_usage(0)
	{
	}

	Buffer::Buffer(Allocator& allocator, std::size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool allowBufferMapping) :
		Object(*allocator.GetDevice()),
		m_allocatedSize(allocSize),
		m_allocator(&allocator),
		m_usage(usage)
	{
		if (Create(allocator, allocSize, usage, memoryUsage, allowBufferMapping) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	Buffer::Buffer(Buffer&& other) noexcept :
		Object(std::move(other)),
		m_mapCount(std::exchange(other.m_mapCount, 0)),
		m_allocatedSize(std::exchange(other.m_allocatedSize, 0)),
		m_allocator(std::exchange(other.m_allocator, nullptr)),
		m_allocation(std::exchange(other.m_allocation, nullptr)),
		m_usage(std::exchange(other.m_usage, 0))
	{
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		std::swap(m_mapCount, other.m_mapCount);
		std::swap(m_allocatedSize, other.m_allocatedSize);
		std::swap(m_allocator, other.m_allocator);
		std::swap(m_allocation, other.m_allocation);
		std::swap(m_usage, other.m_usage);

		Object::operator=(std::move(other));

		return *this;
	}

	VkResult Buffer::Create(Allocator& allocator, std::size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool allowBufferMapping)
	{
		m_device = allocator.GetDevice();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = nullptr;

		bufferInfo.size = allocSize;
		bufferInfo.usage = usage;

		VmaAllocationCreateInfo vmaAllocInfo = {};
		vmaAllocInfo.usage = memoryUsage;
		vmaAllocInfo.flags = allowBufferMapping ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;

		m_lastResult = vmaCreateBuffer(*allocator.Get(), &bufferInfo, &vmaAllocInfo, &m_handle, &m_allocation, nullptr);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vmaCreateBuffer failed VkResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}

	Buffer::~Buffer()
	{
		if (!IsValid())
			return;
		if (m_mapCount != 0)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Trying to destroy a buffer that is mapped");
			return;
		}
		//m_allocator->GetDevice()->WaitIdle();
		vmaDestroyBuffer(*m_allocator->Get(), m_handle, m_allocation);
	}

	void Buffer::Copy(const void* object, std::size_t size, std::size_t padding)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		Byte* data = nullptr;
		if (Map(&data) == false)
			return;
		data += padding;
		std::memcpy(data, object, size);
		UnMap();
	}

	bool Buffer::Map(Byte** data)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		const auto res = vmaMapMemory(*m_allocator->Get(), m_allocation, reinterpret_cast<void**>(data));
		if (res != VK_SUCCESS)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Cannot map buffer");
			return false;
		}
		m_mapCount++;
		return true;
	}

	void Buffer::UnMap()
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		if (m_mapCount == 0)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Trying to destroy a buffer that is mapped");
			return;
		}
		m_mapCount--;
		vmaUnmapMemory(*m_allocator->Get(), m_allocation);
	}

	VmaAllocation Buffer::GetAllocation() const
	{
		return m_allocation;
	}

	Allocator& Buffer::GetAllocator() const
	{
		CCT_ASSERT(m_allocator, "ConcertoGraphics: allocator is null");
		return *m_allocator;
	}

	std::size_t Buffer::GetAllocatedSize() const
	{
		return m_allocatedSize;
	}

	VkBufferUsageFlags Buffer::GetUsage() const
	{
		return m_usage;
	}
}
