//
// Created by arthur on 30/06/2022.
//

#ifndef CONCERTO_GRAPHICS_PIPELINELAYOUT_HPP
#define CONCERTO_GRAPHICS_PIPELINELAYOUT_HPP

#include <vector>
#include <memory>



#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;
	class DescriptorSetLayout;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API PipelineLayout : public Object<VkPipelineLayout>
	{
	public:
		PipelineLayout() = default;
		PipelineLayout(Device& device, const std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts);
		~PipelineLayout() override;

		PipelineLayout(PipelineLayout&&) = default;
		PipelineLayout(const PipelineLayout&) = delete;

		PipelineLayout& operator=(PipelineLayout&&) = default;
		PipelineLayout& operator=(const PipelineLayout&) = delete;

		VkResult Create(Device& device, const std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts);

		[[nodiscard]] const std::vector<std::shared_ptr<DescriptorSetLayout>>& GetDescriptorSetLayouts() const;
	private:
		std::vector<std::shared_ptr<DescriptorSetLayout>> m_descriptorSetLayouts;
	};
} // cct::gfx::vk

#endif //CONCERTO_GRAPHICS_PIPELINELAYOUT_HPP
