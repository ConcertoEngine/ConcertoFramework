//
// Created by arthur on 16/06/22.
//

#ifndef CONCERTO_GRAPHICS_DESCRIPTORSET_HPP
#define CONCERTO_GRAPHICS_DESCRIPTORSET_HPP

#include <memory>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class DescriptorPool;
	class DescriptorSetLayout;
	class Sampler;
	class ImageView;
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API DescriptorSet : public Object<VkDescriptorSet>
	{
	public:
		DescriptorSet() = default;
		DescriptorSet(DescriptorPool& pool, const DescriptorSetLayout& descriptorSetLayout);
		~DescriptorSet() override;

		DescriptorSet(DescriptorSet&&) noexcept;
		DescriptorSet(const DescriptorSet&) = delete;

		DescriptorSet& operator=(DescriptorSet&&) noexcept;
		DescriptorSet& operator=(const DescriptorSet&) = delete;

		VkResult Create(const DescriptorPool& pool, const DescriptorSetLayout& descriptorSetLayout);

		void WriteImageSamplerDescriptor(const Sampler& sampler, const ImageView& imageView, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) const;
	private:
		DescriptorPool* m_pool{};
	};
	using DescriptorSetPtr = std::shared_ptr<DescriptorSet>;
}

#endif //CONCERTO_GRAPHICS_DESCRIPTORSET_HPP
