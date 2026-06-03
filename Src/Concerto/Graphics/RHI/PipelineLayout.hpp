//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_PIPELINELAYOUT_HPP
#define CONCERTO_GRAPHICS_RHI_PIPELINELAYOUT_HPP

#include <memory>
#include <vector>

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class DescriptorSetLayout;

	class CONCERTO_GRAPHICS_RHI_BASE_API PipelineLayout
	{
	public:
		virtual ~PipelineLayout() = default;

		virtual const std::vector<std::shared_ptr<DescriptorSetLayout>>& GetDescriptorSetLayouts() const = 0;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_PIPELINELAYOUT_HPP
