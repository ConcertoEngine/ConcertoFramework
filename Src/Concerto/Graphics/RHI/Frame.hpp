//
// Created by arthur on 05/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_FRAME_HPP
#define CONCERTO_GRAPHICS_RHI_FRAME_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/CommandBuffer.hpp"
#include "Concerto/Graphics/RHI/FrameBuffer.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Frame
	{
	public:
		virtual ~Frame() = default;

		virtual void Present() = 0;
		virtual rhi::CommandBuffer& GetCommandBuffer() = 0;
		virtual std::size_t GetCurrentFrameIndex() = 0;
		virtual rhi::FrameBuffer& GetFrameBuffer() = 0;

	};
}

#endif //CONCERTO_GRAPHICS_RHI_FRAME_HPP