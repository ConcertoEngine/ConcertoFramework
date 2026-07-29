//
// Created by arthur on 03/09/2024.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIBuffer/VkRHIBuffer.hpp"

#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"

namespace cct::gfx::rhi
{
	VkRHIBuffer::VkRHIBuffer(VkRHIDevice& device, rhi::BufferUsageFlags usage, UInt32 allocationSize, bool allowBufferMapping) :
		vk::Buffer(device.GetAllocator(), allocationSize, Converters::ToVulkan(usage), VMA_MEMORY_USAGE_AUTO, allowBufferMapping),
		m_device(device),
		m_allowBufferMapping(allowBufferMapping)
	{
	}

	bool VkRHIBuffer::Map(Byte** data)
	{
		if (!m_allowBufferMapping)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: buffer mapping is not enabled");
			return false;
		}
		return vk::Buffer::Map(data);
		;
	}

	void VkRHIBuffer::UnMap()
	{
		if (!m_allowBufferMapping)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: buffer mapping is not enabled");
			return;
		}
		vk::Buffer::UnMap();
	}
} // namespace cct::gfx::rhi
