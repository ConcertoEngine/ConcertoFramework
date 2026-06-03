//
// Created by arthur on 10/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_TEXTUREIMPORTINFO_HPP
#define CONCERTO_GRAPHICS_RHI_TEXTUREIMPORTINFO_HPP

#include <Concerto/Core/Types/Types.hpp>
#include <Concerto/Graphics/Core/PixelFormat.hpp>

namespace cct::gfx::rhi
{
	enum class ExternalHandleType : UInt32
	{
		D3D11NtHandle, // Windows: HANDLE from D3D11 CreateSharedHandle (NT handle)
		DmaBufFd, // Linux:   DMA-buf file descriptor (int)
	};

	struct TextureImportInfo
	{
		PixelFormat format;
		Int32 width;
		Int32 height;
		ExternalHandleType handleType;
		void* handle;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_TEXTUREIMPORTINFO_HPP
