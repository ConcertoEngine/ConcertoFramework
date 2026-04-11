//
// Created by arthur on 16/06/22.
//

#ifndef CONCERTO_GRAPHICS_DESCRIPTORSETLAYOUT_HPP
#define CONCERTO_GRAPHICS_DESCRIPTORSETLAYOUT_HPP

#include <vector>
#include <memory>


#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API DescriptorSetLayout : public Object<VkDescriptorSetLayout>
	{
	public:
		DescriptorSetLayout() = default;
		DescriptorSetLayout(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
		~DescriptorSetLayout() override;

		DescriptorSetLayout(DescriptorSetLayout&&) = default;
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;

		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(DescriptorSetLayout&&) = default;

		VkResult Create(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);

		[[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const;
		[[nodiscard]] UInt64 GetHash() const;

		static UInt64 GetHash(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

	private:
		std::vector<VkDescriptorSetLayoutBinding> m_bindings;
	};
	using DescriptorSetLayoutPtr = std::shared_ptr<DescriptorSetLayout>;

	DescriptorSetLayoutPtr CONCERTO_GRAPHICS_VULKAN_BACKEND_API MakeDescriptorSetLayout(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
}

#endif //CONCERTO_GRAPHICS_DESCRIPTORSETLAYOUT_HPP
