//
// Created by arthur on 11/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_RENDERGRAPH_RESOURCE_HPP
#define CONCERTO_GRAPHICS_RHI_RENDERGRAPH_RESOURCE_HPP

#include <limits>

#include "Concerto/Graphics/RHI/Enums.hpp"

namespace cct::gfx::rhi
{
	struct RGTextureHandle
	{
		UInt16 index = std::numeric_limits<UInt16>::max();
		UInt16 version = 0;
		[[nodiscard]] bool IsValid() const
		{
			return index != std::numeric_limits<UInt16>::max();
		}
		bool operator==(const RGTextureHandle&) const = default;
	};

	struct RGBufferHandle
	{
		UInt16 index = std::numeric_limits<UInt16>::max();
		UInt16 version = 0;
		[[nodiscard]] bool IsValid() const
		{
			return index != std::numeric_limits<UInt16>::max();
		}
		bool operator==(const RGBufferHandle&) const = default;
	};

	struct RGTextureDesc
	{
		UInt32 width = 0;
		UInt32 height = 0;
		PixelFormat format = PixelFormat::RGBA8_SRGB;
		bool isDepth = false;
		const char* name = "";
		// Storage image (compute shader Read/Write) instead of a sampled/color-attachment texture.
		// Allocated via Device::CreateStorageTexture and tracked in ImageLayout::General.
		// Appended last so existing positional aggregate-init call sites (e.g. Compositor.cpp's
		// {width, height, format, isDepth, "name"}) keep binding "name" to the name field.
		bool storage = false;
	};

	struct RGBufferDesc
	{
		UInt32 size = 0;
		BufferUsageFlags usage = {};
		const char* name = "";
	};

	enum class RGPassType
	{
		Graphics,
		Compute,
		Transfer
	};
	enum class RGResourceAccess
	{
		Read,
		Write
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_RENDERGRAPH_RESOURCE_HPP
