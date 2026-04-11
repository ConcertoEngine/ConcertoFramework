//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_FRAMEBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_FRAMEBUFFER_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class Texture;
	class RenderPass;
	class Device;

	class CONCERTO_GRAPHICS_RHI_BASE_API FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;
		virtual UInt32 GetWidth() const = 0;
		virtual UInt32 GetHeight() const = 0;
	};
}


#endif //CONCERTO_GRAPHICS_RHI_FRAMEBUFFER_HPP