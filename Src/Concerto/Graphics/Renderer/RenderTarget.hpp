#ifndef CONCERTO_GRAPHICS_RENDERER_RENDERTARGET_HPP
#define CONCERTO_GRAPHICS_RENDERER_RENDERTARGET_HPP

#include <memory>

#include "Concerto/Graphics/Core/PixelFormat.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"

namespace cct::gfx
{
	struct RenderTarget
	{
		std::shared_ptr<rhi::Texture> colorTexture;
		rhi::ImageLayout currentLayout = rhi::ImageLayout::Undefined;
		rhi::ImageLayout exportLayout = rhi::ImageLayout::ShaderReadOnlyOptimal;
		PixelFormat colorFormat = PixelFormat::RGBA8_SRGB;
		PixelFormat depthFormat = PixelFormat::D32f;
		UInt32 width = 0;
		UInt32 height = 0;
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_RENDERER_RENDERTARGET_HPP
