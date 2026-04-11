//
// Created by arthur on 16/06/22.
//

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSet/DescriptorSet.hpp"

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSetLayout/DescriptorSetLayout.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorPool/DescriptorPool.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Sampler/Sampler.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ImageView/ImageView.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"

namespace cct::gfx::vk
{
	DescriptorSet::DescriptorSet(DescriptorPool& pool, const DescriptorSetLayout& descriptorSetLayout) :
		Object(*pool.GetDevice()),
		m_pool(&pool)
	{
		if (Create(pool, descriptorSetLayout) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	DescriptorSet::~DescriptorSet()
	{
		if (!IsValid())
			return;
		if (m_pool == nullptr)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Trying to destroy a descriptor set with an invalid pool");
			return;
		}
		m_device->vkFreeDescriptorSets(*m_device->Get(), *m_pool->Get(), 1, &m_handle);
	}

	VkResult DescriptorSet::Create(const DescriptorPool& pool, const DescriptorSetLayout& descriptorSetLayout)
	{
		m_device = pool.GetDevice();

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = nullptr;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = *pool.Get();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = descriptorSetLayout.Get();

		m_lastResult = m_device->vkAllocateDescriptorSets(*m_device->Get(), &allocInfo, &m_handle);

		return m_lastResult;
	}

	DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept : Object<VkDescriptorSet>(std::move(other))
	{
		m_pool = std::exchange(other.m_pool, nullptr);
	}

	DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept
	{
		m_pool = std::exchange(other.m_pool, nullptr);
		return *this;
	}

	void DescriptorSet::WriteImageSamplerDescriptor(const Sampler& sampler, const ImageView& imageView, VkImageLayout imageLayout) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		VkDescriptorImageInfo imageBufferInfo;
		imageBufferInfo.sampler = *sampler.Get();
		imageBufferInfo.imageView = *imageView.Get();
		imageBufferInfo.imageLayout = imageLayout;

		const VkWriteDescriptorSet texture1 = VulkanInitializer::WriteDescriptorImage(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, m_handle, &imageBufferInfo, 0);

		m_device->vkUpdateDescriptorSets(*m_device->Get(), 1, &texture1, 0, nullptr);
	}
}
