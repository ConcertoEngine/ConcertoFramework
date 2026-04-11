//
// Created by arthur on 16/02/2023.
//

#include "Concerto/Graphics/Backend/Vulkan/DescriptorBuilder/DescriptorBuilder.hpp"

#include "Concerto/Graphics/Backend/Vulkan/DescriptorAllocator/DescriptorAllocator.hpp"
#include "Concerto/Graphics/Backend/Vulkan/DescriptorLayoutCache.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSet/DescriptorSet.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSetLayout/DescriptorSetLayout.hpp"

namespace cct::gfx::vk
{

	DescriptorBuilder::DescriptorBuilder(DescriptorLayoutCache& layoutCache, DescriptorAllocator& allocator) :
		m_cache(layoutCache), m_alloc(allocator)
	{

	}

	DescriptorBuilder& DescriptorBuilder::BindBuffer(UInt32 binding,
		VkDescriptorBufferInfo* bufferInfo,
		VkDescriptorType type,
		VkShaderStageFlags stageFlags)
	{
		VkDescriptorSetLayoutBinding newBinding{};

		newBinding.descriptorCount = 1;
		newBinding.descriptorType = type;
		newBinding.pImmutableSamplers = nullptr;
		newBinding.stageFlags = stageFlags;
		newBinding.binding = binding;

		m_bindings.push_back(newBinding);

		VkWriteDescriptorSet newWrite{};
		newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		newWrite.pNext = nullptr;

		newWrite.descriptorCount = 1;
		newWrite.descriptorType = type;
		newWrite.pBufferInfo = bufferInfo;
		newWrite.dstBinding = binding;

		m_writes.push_back(newWrite);
		return *this;
	}

	DescriptorBuilder& DescriptorBuilder::BindImage(UInt32 binding,
		VkDescriptorImageInfo* imageInfo,
		VkDescriptorType type,
		VkShaderStageFlags stageFlags)
	{
		VkDescriptorSetLayoutBinding newBinding{};

		newBinding.descriptorCount = 1;
		newBinding.descriptorType = type;
		newBinding.pImmutableSamplers = nullptr;
		newBinding.stageFlags = stageFlags;
		newBinding.binding = binding;

		m_bindings.push_back(newBinding);

		VkWriteDescriptorSet newWrite{};
		newWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		newWrite.pNext = nullptr;

		newWrite.descriptorCount = 1;
		newWrite.descriptorType = type;
		newWrite.pImageInfo = imageInfo;
		newWrite.dstBinding = binding;

		m_writes.push_back(newWrite);
		return *this;
	}

	bool DescriptorBuilder::Build(std::shared_ptr<DescriptorSet>& set, std::shared_ptr<DescriptorSetLayout>& layout)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.pNext = nullptr;

		layoutInfo.pBindings = m_bindings.data();
		layoutInfo.bindingCount = static_cast<UInt32>(m_bindings.size());

		layout = m_cache.GetLayout(layoutInfo);

		const bool success = m_alloc.Allocate(set, *layout);
		if (!success)
			return false;

		for (VkWriteDescriptorSet& w : m_writes)
			w.dstSet = *set->Get();

		m_alloc.GetDevice().vkUpdateDescriptorSets(*m_alloc.GetDevice().Get(), static_cast<UInt32>(m_writes.size()), m_writes.data(), 0, nullptr);
		m_bindings.clear();
		m_writes.clear();
		return true;
	}

	bool DescriptorBuilder::Build(std::shared_ptr<DescriptorSet>& set)
	{
		DescriptorSetLayoutPtr layout;
		return Build(set, layout);
	}
}