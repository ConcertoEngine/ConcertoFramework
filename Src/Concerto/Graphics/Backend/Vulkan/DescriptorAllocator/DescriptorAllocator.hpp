//
// Created by arthur on 16/02/2023.
//

#ifndef CONCERTO_GRAPHICS_INCLUDE_DESCRIPTORALLOCATOR_HPP_
#define CONCERTO_GRAPHICS_INCLUDE_DESCRIPTORALLOCATOR_HPP_

#include <vector>
#include <unordered_map>
#include <mutex>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorPool/DescriptorPool.hpp"

namespace cct::gfx::vk
{
	class DescriptorSet;
	class DescriptorSetLayout;
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API DescriptorAllocator
	{
	 public:
		struct PoolSizes {
			std::vector<std::pair<VkDescriptorType,float>> sizes =
				{
					{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
					{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
					{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
					{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
					{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
					{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
					{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
					{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
				};
		};
		explicit DescriptorAllocator(Device &device);
		bool Allocate(DescriptorSetPtr &descriptorSet, const DescriptorSetLayout &layout);
		bool AllocateWithoutCache(DescriptorSetPtr& descriptorSet, const DescriptorSetLayout& layout);
	 	/**
	 	 * @brief Reset the allocator, resetting all the descriptors in the pool.
	 	 */
		void Reset();
		[[nodiscard]] Device &GetDevice() const;
		DescriptorPoolPtr GetDescriptorPool();

	private:
		DescriptorPoolPtr CreatePool(VkDescriptorPoolCreateFlags flags);
		DescriptorPoolPtr GetPool();

		DescriptorSetPtr TryAllocate(const DescriptorSetLayout& layout);
		Device *m_device;
		PoolSizes m_poolSizes;
		std::mutex m_usedPoolMutex;
		std::unordered_map<std::thread::id, std::vector<DescriptorPoolPtr>> m_usedPools;
		std::vector<DescriptorPoolPtr> m_freePools;
		std::unordered_map<UInt64 /*Hash of bindings*/, DescriptorSetPtr> m_cache;
	};
}

#endif //CONCERTO_GRAPHICS_INCLUDE_DESCRIPTORALLOCATOR_HPP_
