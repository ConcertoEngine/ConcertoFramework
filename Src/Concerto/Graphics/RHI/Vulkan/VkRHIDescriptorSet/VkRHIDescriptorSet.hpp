//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSET_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSET_HPP

#include <memory>
#include <unordered_map>
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/DescriptorSet.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSet/DescriptorSet.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Sampler/Sampler.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIDescriptorSet : public DescriptorSet
	{
	public:
		VkRHIDescriptorSet(vk::DescriptorSetPtr vkDescriptorSet,
		                   std::shared_ptr<DescriptorSetLayout> layout);

		~VkRHIDescriptorSet() override;

		VkRHIDescriptorSet(const VkRHIDescriptorSet&) = delete;
		VkRHIDescriptorSet& operator=(const VkRHIDescriptorSet&) = delete;
		VkRHIDescriptorSet(VkRHIDescriptorSet&&) noexcept = default;
		VkRHIDescriptorSet& operator=(VkRHIDescriptorSet&&) noexcept = default;

		void BindBuffer(UInt32 binding, const Buffer& buffer, UInt32 offset = 0, UInt32 range = 0) override;
		void BindTexture(UInt32 binding, const Texture& texture) override;
		const std::shared_ptr<DescriptorSetLayout>& GetLayout() const override;
		const vk::DescriptorSetPtr& Get() const;

	private:
		vk::DescriptorSetPtr m_vkDescriptorSet;
		std::shared_ptr<DescriptorSetLayout> m_layout;
		// One sampler per binding, created once and reused every frame.
		std::unordered_map<UInt32, std::unique_ptr<vk::Sampler>> m_samplerCache;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIDESCRIPTORSET_HPP
