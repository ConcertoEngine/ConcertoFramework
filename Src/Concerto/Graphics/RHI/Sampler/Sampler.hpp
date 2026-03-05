//
// Created by arthur on 05/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_SAMPLER_HPP
#define CONCERTO_GRAPHICS_RHI_SAMPLER_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Sampler
	{
	public:
		virtual ~Sampler() = default;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_SAMPLER_HPP
