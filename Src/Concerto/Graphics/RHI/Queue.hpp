//
// Created by arthur on 09/04/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_QUEUE_HPP
#define CONCERTO_GRAPHICS_RHI_QUEUE_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class CommandBuffer;
	class Fence;

	class CONCERTO_GRAPHICS_RHI_BASE_API Queue
	{
	public:
		virtual ~Queue() = default;

		virtual void Submit(CommandBuffer& cmdBuf, Fence* fence = nullptr) = 0;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_QUEUE_HPP
