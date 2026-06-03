//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_PIPELINE_HPP
#define CONCERTO_GRAPHICS_RHI_PIPELINE_HPP

#include <memory>

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class PipelineLayout;

	class CONCERTO_GRAPHICS_RHI_BASE_API Pipeline
	{
	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_PIPELINE_HPP
