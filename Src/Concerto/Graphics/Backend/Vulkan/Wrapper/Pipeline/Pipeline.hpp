//
// Created by arthur on 10/06/22.
//

#ifndef CONCERTO_GRAPHICS_PIPELINE_HPP
#define CONCERTO_GRAPHICS_PIPELINE_HPP

#include <memory>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PipelineInfo/PipelineInfo.hpp"

namespace cct::gfx::vk
{
	class Device;
	class PipelineLayout;
	class RenderPass;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API Pipeline : public Object<VkPipeline>
	{
	public:
		Pipeline() = default;
		Pipeline(Device& device, PipelineInfo pipeLineInfo, const RenderPass& renderPass);
		~Pipeline() override;

		Pipeline(Pipeline&&) = default;
		Pipeline(const Pipeline&) = delete;

		Pipeline& operator=(Pipeline&&) = default;
		Pipeline& operator=(const Pipeline&) = delete;

		VkResult Create(Device& device, const PipelineInfo& pipeLineInfo, const RenderPass& renderPass);

		[[nodiscard]] const PipelineLayout& GetPipelineLayout() const;
		struct CreateInfo
		{
			VkPipelineViewportStateCreateInfo viewportState;
			VkPipelineColorBlendStateCreateInfo colorBlend;
			VkGraphicsPipelineCreateInfo pipelineCreateInfo;
		};

	private:
		PipelineInfo m_pipelineInfo;
		CreateInfo m_createInfo;
		const RenderPass* m_renderPass;

		[[nodiscard]] static VkPipelineViewportStateCreateInfo BuildViewportState();
		[[nodiscard]] static VkPipelineColorBlendStateCreateInfo BuildColorBlendState();
		[[nodiscard]] VkResult BuildPipeline();
	};

	using PipelinePtr = std::shared_ptr<Pipeline>;
} // namespace cct::gfx::vk

#endif // CONCERTO_GRAPHICS_PIPELINE_HPP
