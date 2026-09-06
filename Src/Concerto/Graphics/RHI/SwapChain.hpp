//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_SWAPCHAIN_HPP
#define CONCERTO_GRAPHICS_RHI_SWAPCHAIN_HPP

#include <memory>

#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Frame.hpp"

namespace cct::gfx::rhi
{
	class Texture;

	class CONCERTO_GRAPHICS_RHI_BASE_API SwapChain
	{
	public:
		inline SwapChain(PixelFormat pixelFormat, PixelFormat depthPixelFormat);
		virtual ~SwapChain() = default;

		virtual Vector2u GetExtent() const = 0;
		virtual UInt32 GetImageCount() const = 0;
		virtual rhi::Frame& AcquireFrame() = 0;
		virtual void WaitAll() const = 0;

		virtual std::shared_ptr<Texture> GetColorTexture(UInt32 imageIndex) = 0;

		[[nodiscard]] inline PixelFormat GetPixelFormat() const;
		[[nodiscard]] inline PixelFormat GetDepthPixelFormat() const;

	private:
		PixelFormat m_pixelFormat;
		PixelFormat m_depthPixelFormat;
	};
} // namespace cct::gfx::rhi

#include "Concerto/Graphics/RHI/SwapChain.inl"

#endif // CONCERTO_GRAPHICS_RHI_SWAPCHAIN_HPP