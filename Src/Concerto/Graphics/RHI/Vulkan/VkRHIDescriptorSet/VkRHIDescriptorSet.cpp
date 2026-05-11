//
// Created by arthur on 24/10/2025.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSet/VkRHIDescriptorSet.hpp"

#include <algorithm>

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Sampler/Sampler.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIBuffer/VkRHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSetLayout/VkRHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"

namespace cct::gfx::rhi
{
	VkRHIDescriptorSet::VkRHIDescriptorSet(vk::DescriptorSetPtr vkDescriptorSet,
										   std::shared_ptr<DescriptorSetLayout> layout) :
		m_vkDescriptorSet(std::move(vkDescriptorSet)),
		m_layout(std::move(layout))
	{
	}

	VkRHIDescriptorSet::~VkRHIDescriptorSet() = default;

	void VkRHIDescriptorSet::BindBuffer(UInt32 binding, const Buffer& buffer, UInt32 offset, UInt32 range)
	{
		auto* vkBuffer = dynamic_cast<const VkRHIBuffer*>(&buffer);
		CCT_ASSERT(vkBuffer, "VkRHIDescriptorSet::BindBuffer expects a VkRHIBuffer");

		auto vkLayout = std::dynamic_pointer_cast<VkRHIDescriptorSetLayout>(m_layout);
		CCT_ASSERT(vkLayout, "Invalid descriptor set layout type for Vulkan backend");

		// Locate binding description to determine descriptor type
		const auto& bindings = vkLayout->GetBindings();
		auto bindingIt = std::find_if(bindings.begin(), bindings.end(), [binding](const auto& b)
									  { return b.binding == binding; });
		CCT_ASSERT(bindingIt != bindings.end(), "Binding index not found in layout");

		const VkDescriptorType vkDescriptorType = Converters::ToVulkan(bindingIt->descriptorType);

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = *vkBuffer->Get();
		bufferInfo.offset = offset;
		const std::size_t fullRange = vkBuffer->GetAllocatedSize();
		bufferInfo.range = (range == 0 ? static_cast<VkDeviceSize>(fullRange - offset) : static_cast<VkDeviceSize>(range));

		const VkWriteDescriptorSet write = VulkanInitializer::WriteDescriptorBuffer(vkDescriptorType, *m_vkDescriptorSet->Get(), &bufferInfo, binding);

		auto* device = m_vkDescriptorSet->GetDevice();
		CCT_ASSERT(device, "DescriptorSet device is null");
		device->vkUpdateDescriptorSets(*device->Get(), 1, &write, 0, nullptr);
	}

	void VkRHIDescriptorSet::BindTexture(UInt32 binding, const Texture& texture)
	{
		auto* vkTexture = dynamic_cast<const VkRHITexture*>(&texture);
		CCT_ASSERT(vkTexture, "VkRHIDescriptorSet::BindTexture expects a VkRHITexture");

		auto vkLayout = std::dynamic_pointer_cast<VkRHIDescriptorSetLayout>(m_layout);
		CCT_ASSERT(vkLayout, "Invalid descriptor set layout type for Vulkan backend");

		const auto& bindings = vkLayout->GetBindings();
		auto bindingIt = std::find_if(bindings.begin(), bindings.end(), [binding](const auto& b)
									  { return b.binding == binding; });
		CCT_ASSERT(bindingIt != bindings.end(), "Binding index not found in layout");

		const VkDescriptorType vkDescriptorType = Converters::ToVulkan(bindingIt->descriptorType);

		auto* device = m_vkDescriptorSet->GetDevice();
		CCT_ASSERT(device, "DescriptorSet device is null");

		auto& sampler = m_samplerCache[binding];
		if (!sampler)
		{
			sampler = std::make_unique<vk::Sampler>(*device, VK_FILTER_LINEAR);
		}

		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = *sampler->Get();
		imageInfo.imageView = *vkTexture->GetImageView().Get();
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		const VkWriteDescriptorSet write = VulkanInitializer::WriteDescriptorImage(vkDescriptorType, *m_vkDescriptorSet->Get(), &imageInfo, binding);
		device->vkUpdateDescriptorSets(*device->Get(), 1, &write, 0, nullptr);
	}

	const std::shared_ptr<DescriptorSetLayout>& VkRHIDescriptorSet::GetLayout() const
	{
		return m_layout;
	}

	const vk::DescriptorSetPtr& VkRHIDescriptorSet::Get() const
	{
		return m_vkDescriptorSet;
	}
} // namespace cct::gfx::rhi
