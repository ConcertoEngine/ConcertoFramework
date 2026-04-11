//
// Created by arthur on 16/02/2023.
//

#ifndef CONCERTO_GRAPHICS_INCLUDE_DESCRIPTORLAYOUTCACHE_HPP_
#define CONCERTO_GRAPHICS_INCLUDE_DESCRIPTORLAYOUTCACHE_HPP_

#include <unordered_map>
#include <algorithm>



#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSetLayout/DescriptorSetLayout.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{
//	template<typename HashMap = std::unordered_map<void, void>>
	class DescriptorLayoutCache
	{
	 public:
		struct DescriptorLayoutInfo
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;

			bool operator==(const DescriptorLayoutInfo& other) const
			{
				if (other.bindings.size() != bindings.size())
					return false;
				for (int i = 0; i < bindings.size(); i++)
				{
					if (other.bindings[i].binding != bindings[i].binding)
						return false;
					if (other.bindings[i].descriptorType != bindings[i].descriptorType)
						return false;
					if (other.bindings[i].descriptorCount != bindings[i].descriptorCount)
						return false;
					if (other.bindings[i].stageFlags != bindings[i].stageFlags)
						return false;
				}
				return true;
			}

			[[nodiscard]] std::size_t hash() const
			{
				std::size_t result = std::hash<std::size_t>()(bindings.size());
				for (const VkDescriptorSetLayoutBinding& b : bindings)
				{
					std::size_t	binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;
					result ^= std::hash<std::size_t>()(binding_hash);
				}
				return result;
			}
		};
	 private:
		struct DescriptorLayoutHash
		{
			std::size_t operator()(const DescriptorLayoutInfo& descriptorLayoutInfo) const
			{
				return descriptorLayoutInfo.hash();
			}
		};
	 public:
		explicit DescriptorLayoutCache(Device& device) :
			m_device(&device)
		{
		}

		DescriptorSetLayoutPtr GetLayout(const VkDescriptorSetLayoutCreateInfo& createInfo)
		{
			DescriptorLayoutInfo layoutInfo;
			layoutInfo.bindings.reserve(createInfo.bindingCount);
			bool isSorted = true;
			UInt32 lastBinding = -1;

			for (UInt32 i = 0; i < createInfo.bindingCount; i++)
			{
				layoutInfo.bindings.push_back(createInfo.pBindings[i]);

				if (createInfo.pBindings[i].binding > lastBinding)
					lastBinding = createInfo.pBindings[i].binding;
				else isSorted = false;
			}

			if (!isSorted)
			{
				std::sort(layoutInfo.bindings.begin(),
					layoutInfo.bindings.end(),
					[](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b)
					{
					  return a.binding < b.binding;
					});
			}

			auto it = m_layoutsCache.find(layoutInfo);
			if (it != m_layoutsCache.end())
				return (*it).second;
			auto [elementIt, m_] =
				m_layoutsCache.emplace(layoutInfo, MakeDescriptorSetLayout(*m_device, layoutInfo.bindings));
			return elementIt->second;
		}

	 private:
		Device* m_device;
		std::unordered_map<DescriptorLayoutInfo, DescriptorSetLayoutPtr, DescriptorLayoutHash> m_layoutsCache;
	};
}
#endif //CONCERTO_GRAPHICS_INCLUDE_DESCRIPTORLAYOUTCACHE_HPP_
