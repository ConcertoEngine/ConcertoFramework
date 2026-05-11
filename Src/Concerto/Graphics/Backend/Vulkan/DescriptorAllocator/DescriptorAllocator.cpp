//
// Created by arthur on 16/02/2023.
//

#define DESCRIPTOR_POOL_SIZE 1000

#include "Concerto/Graphics/Backend/Vulkan/DescriptorAllocator/DescriptorAllocator.hpp"

#include <sstream>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSet/DescriptorSet.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSetLayout/DescriptorSetLayout.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{
	DescriptorAllocator::DescriptorAllocator(Device& device) :
		m_device(&device)
	{
	}

	bool DescriptorAllocator::Allocate(DescriptorSetPtr& descriptorSet, const DescriptorSetLayout& layout)
	{
		auto it = m_cache.find(layout.GetHash());
		if (it != m_cache.end())
		{
			descriptorSet = it->second;
			return true;
		}
		if (auto set = TryAllocate(layout))
		{
			descriptorSet = set;
			m_cache.emplace(layout.GetHash(), set);
			return true;
		}
		return false;
	}

	bool DescriptorAllocator::AllocateWithoutCache(DescriptorSetPtr& descriptorSet, const DescriptorSetLayout& layout)
	{
		descriptorSet = TryAllocate(layout);
		return descriptorSet != nullptr;
	}

	DescriptorPoolPtr DescriptorAllocator::CreatePool(VkDescriptorPoolCreateFlags)
	{
		CCT_PROFILER_SCOPE();
		std::vector<VkDescriptorPoolSize> sizes;
		sizes.reserve(m_poolSizes.sizes.size());
		for (const auto& [descriptorType, number] : m_poolSizes.sizes)
		{
			// Validate descriptor type is in valid range
			if (descriptorType > VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT && descriptorType < 1000000000)
			{
				CCT_ASSERT_FALSE("ConcertoGraphics: Invalid VkDescriptorType {}, skipping", static_cast<int>(descriptorType));
				continue;
			}
			sizes.push_back({descriptorType, static_cast<UInt32>(number * DESCRIPTOR_POOL_SIZE)});
		}
		return std::make_shared<DescriptorPool>(*m_device, sizes);
	}

	DescriptorPoolPtr DescriptorAllocator::GetPool()
	{
		if (m_freePools.empty())
			return CreatePool(0);
		DescriptorPoolPtr pool = m_freePools.back();
		m_freePools.pop_back();
		return pool;
	}

	DescriptorSetPtr DescriptorAllocator::TryAllocate(const DescriptorSetLayout& layout)
	{
		CCT_PROFILER_SCOPE();
		std::lock_guard m_(m_usedPoolMutex);
		DescriptorPoolPtr currentPool;
		std::shared_ptr<DescriptorSet> descriptorSet;
		auto createDescriptorSet = [&]() -> std::shared_ptr<DescriptorSet>
		{
			return std::make_shared<DescriptorSet>(*currentPool, layout);
		};

		auto it = m_usedPools.find(std::this_thread::get_id());
		if (it == m_usedPools.end())
		{
			currentPool = GetPool();
			descriptorSet = createDescriptorSet();
			m_usedPools.emplace(std::this_thread::get_id(), std::vector{currentPool});
		}
		else
		{
			// Try to create a descriptor set from an existing pool
			bool succeed = false;
			for (auto& pool : it->second)
			{
				currentPool = pool;
				descriptorSet = createDescriptorSet();
				succeed = descriptorSet && descriptorSet->GetLastResult() == VK_SUCCESS;
				if (succeed)
					break;
			}

			// Create a new pool and allocate a descriptor set within
			if (succeed == false)
			{
				currentPool = GetPool();
				descriptorSet = createDescriptorSet();
				succeed = descriptorSet && descriptorSet->GetLastResult() == VK_SUCCESS;
				if (succeed == false)
				{
					CCT_ASSERT_FALSE("Failed to allocate descriptor set in a newly created pool"); // FIXME
					return nullptr;
				}
				it->second.emplace_back(currentPool);
			}
		}

		if (descriptorSet && descriptorSet->GetLastResult() == VK_SUCCESS)
		{
			m_cache.emplace(layout.GetHash(), descriptorSet);
			return descriptorSet;
		}
		return nullptr;
	}

	void DescriptorAllocator::Reset()
	{
		std::lock_guard m_(m_usedPoolMutex);
		auto it = m_usedPools.find(std::this_thread::get_id());
		if (it != m_usedPools.end())
		{
			for (auto& pool : it->second)
			{
				pool->Reset();
				m_freePools.push_back(pool);
			}
			m_usedPools.erase(it);
		}
	}

	Device& DescriptorAllocator::GetDevice() const
	{
		CCT_ASSERT(m_device != nullptr, "Vulkan device is null");
		return *m_device;
	}

	DescriptorPoolPtr DescriptorAllocator::GetDescriptorPool()
	{
		/*std::lock_guard m_(m_usedPoolMutex);
		auto it = m_usedPools.find(std::this_thread::get_id());
		if (it == m_usedPools.end())
			return GetPool();
		return it->second;*/
		CCT_ASSERT_FALSE("FIXME");
		return nullptr;
	}
} // namespace cct::gfx::vk
