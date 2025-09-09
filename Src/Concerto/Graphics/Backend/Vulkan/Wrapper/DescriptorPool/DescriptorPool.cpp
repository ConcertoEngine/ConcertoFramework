//
// Created by arthur on 16/06/22.
//

#include <cassert>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorPool/DescriptorPool.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{
	DescriptorPool::DescriptorPool(Device& device) : Object(device)
	{
		std::vector<VkDescriptorPoolSize> poolSizes =
		{
				{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1000 },
				{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = 1000 },
				{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1000 },
				{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 1000 }
		};

		if (Create(device, std::move(poolSizes)) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	DescriptorPool::DescriptorPool(Device& device, std::vector<VkDescriptorPoolSize> poolSizes) : Object<VkDescriptorPool>(device)
	{
		if (Create(device, std::move(poolSizes)) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	DescriptorPool::~DescriptorPool()
	{
		if (!IsValid())
			return;
		m_device->vkDestroyDescriptorPool(*m_device->Get(), m_handle, nullptr);
	}

	VkResult DescriptorPool::Create(Device& device, std::vector<VkDescriptorPoolSize> poolSizes)
	{
		m_device = &device;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets = 0xFFF;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<UInt32>(poolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		m_lastResult = device.vkCreateDescriptorPool(*device.Get(), &descriptorPoolCreateInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateDescriptorPool failed VkResult={}", static_cast<const int>(m_lastResult));

		return m_lastResult;
	}

	DescriptorSet DescriptorPool::AllocateDescriptorSet(DescriptorSetLayout& setLayout)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		return { *this, setLayout };
	}

	void DescriptorPool::Reset() const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		m_device->vkResetDescriptorPool(*m_device->Get(), m_handle, 0);
	}
}