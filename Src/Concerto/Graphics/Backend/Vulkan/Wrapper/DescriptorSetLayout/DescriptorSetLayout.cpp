//
// Created by arthur on 16/06/22.
//

#include <cassert>
#include <stdexcept>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSetLayout/DescriptorSetLayout.hpp"

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{

	DescriptorSetLayout::DescriptorSetLayout(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings) :
		Object(device),
		m_bindings(bindings)
	{
		if (Create(device, bindings) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		if (!IsValid())
			return;
		m_device->vkDestroyDescriptorSetLayout(*m_device->Get(), m_handle, nullptr);
	}

	VkResult DescriptorSetLayout::Create(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		m_device = &device;
		m_bindings = bindings;

		VkDescriptorSetLayoutCreateInfo createInfo = {};
		createInfo.flags = 0;
		createInfo.pNext = nullptr;
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.pBindings = m_bindings.data();
		createInfo.bindingCount = static_cast<UInt32>(m_bindings.size());

		m_lastResult = m_device->vkCreateDescriptorSetLayout(*m_device->Get(), &createInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateDescriptorSetLayout failed VkResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}

	const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSetLayout::GetBindings() const
	{
		return m_bindings;
	}

	UInt64 DescriptorSetLayout::GetHash() const
	{
		return GetHash(m_bindings);
	}

	UInt64 DescriptorSetLayout::GetHash(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		std::hash<UInt32> hasher;
		UInt64 hash = 0;

		for (const auto& binding : bindings) {
			hash ^= hasher(binding.binding) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			hash ^= hasher(binding.descriptorType) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			hash ^= hasher(binding.descriptorCount) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			hash ^= hasher(binding.stageFlags) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
		return hash;
	}

	DescriptorSetLayoutPtr MakeDescriptorSetLayout(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		return std::make_shared<DescriptorSetLayout>(device, bindings);
	}
}
