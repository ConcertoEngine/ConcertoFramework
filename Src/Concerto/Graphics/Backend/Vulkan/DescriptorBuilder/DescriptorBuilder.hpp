//
// Created by arthur on 16/02/2023.
//

#ifndef CONCERTO_GRAPHICS_INCLUDE_DESCRIPTORBUILDER_HPP_
#define CONCERTO_GRAPHICS_INCLUDE_DESCRIPTORBUILDER_HPP_

#include <vector>
#include <memory>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"

namespace cct::gfx::vk
{
	class DescriptorSetLayout;
	class DescriptorSet;
	class DescriptorLayoutCache;
	class DescriptorAllocator;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API DescriptorBuilder
	{
	 public:
		DescriptorBuilder(DescriptorLayoutCache& layoutCache, DescriptorAllocator& allocator);

		DescriptorBuilder& BindBuffer(UInt32 binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);
		DescriptorBuilder& BindImage(UInt32 binding, VkDescriptorImageInfo* imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);

		bool Build(std::shared_ptr<DescriptorSet>& set, std::shared_ptr<DescriptorSetLayout>& layout);
		bool Build(std::shared_ptr<DescriptorSet>& set);
	 private:

		std::vector<VkWriteDescriptorSet> m_writes;
		std::vector<VkDescriptorSetLayoutBinding> m_bindings;

		DescriptorLayoutCache& m_cache;
		DescriptorAllocator& m_alloc;
	};
}
#endif //CONCERTO_GRAPHICS_INCLUDE_DESCRIPTORBUILDER_HPP_
