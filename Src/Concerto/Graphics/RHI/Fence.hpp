//
// Created by arthur on 09/04/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_FENCE_HPP
#define CONCERTO_GRAPHICS_RHI_FENCE_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Fence
	{
	public:
		virtual ~Fence() = default;

		virtual void Wait() = 0;
		virtual void Reset() = 0;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_FENCE_HPP
