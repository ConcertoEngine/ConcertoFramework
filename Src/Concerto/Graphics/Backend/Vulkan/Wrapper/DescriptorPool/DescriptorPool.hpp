//
// Created by arthur on 16/06/22.
//

#ifndef CONCERTO_GRAPHICS_DESCRIPTORPOOL_HPP
#define CONCERTO_GRAPHICS_DESCRIPTORPOOL_HPP

#include <vector>
#include <memory>


#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSet/DescriptorSet.hpp"

namespace cct::gfx::vk
{
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API DescriptorPool : public Object<VkDescriptorPool>
	{
	 public:
		DescriptorPool() = default;
		explicit DescriptorPool(Device& device);
		DescriptorPool(Device& device, std::vector<VkDescriptorPoolSize> poolSizes);
		~DescriptorPool() override;

		DescriptorPool(DescriptorPool&&) = default;
		DescriptorPool(const DescriptorPool&) = delete;

		DescriptorPool& operator=(DescriptorPool&&) = default;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		VkResult Create(Device& device, std::vector<VkDescriptorPoolSize> poolSizes);

		DescriptorSet AllocateDescriptorSet(DescriptorSetLayout& setLayout);

		void Reset() const;
	};
	using DescriptorPoolPtr = std::shared_ptr<DescriptorPool>;
}

#endif //CONCERTO_GRAPHICS_DESCRIPTORPOOL_HPP
