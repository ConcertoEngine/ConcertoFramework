//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHISWAPCHAIN_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHISWAPCHAIN_HPP

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Fence/Fence.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/SwapChain/SwapChain.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandPool/Dx12RHICommandPool.hpp"
#include "Concerto/Graphics/RHI/SwapChain.hpp"

namespace cct::gfx::rhi
{
	class Dx12RHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHISwapChain : public rhi::SwapChain, public dx12::SwapChain
	{
	public:
		Dx12RHISwapChain(rhi::Dx12RHIDevice& device, Window& window, PixelFormat pixelFormat, PixelFormat depthPixelFormat);

		Vector2u GetExtent() const override;
		UInt32 GetImageCount() const override;
		rhi::Frame& AcquireFrame() override;
		void WaitAll() const override;
		std::shared_ptr<Texture> GetColorTexture(UInt32 imageIndex) override;
		CommandPool& GetCommandPool();

	private:
		class SwapChainFrame : public rhi::Frame
		{
		public:
			SwapChainFrame(Dx12RHISwapChain& owner, UInt32 imageIndex);

			void Present() override;
			rhi::CommandBuffer& GetCommandBuffer() override;
			std::size_t GetCurrentFrameIndex() override;
			void Wait() const;

			const dx12::Fence& GetRenderFence() const;

			static constexpr UInt32 InvalidFrameIndex = std::numeric_limits<UInt32>::max();

		private:
			Dx12RHICommandBuffer m_commandBuffer;
			dx12::Fence m_renderFence;
			Dx12RHISwapChain* m_owner = nullptr;
			UInt32 m_imageIndex = 0;
		};

		void CreateColorTextures();

		Dx12RHIDevice* m_rhiDevice;
		UInt64 m_currentFrameIndex;
		std::vector<SwapChainFrame> m_frames;
		std::vector<std::shared_ptr<Texture>> m_colorTextures;
		Dx12RHICommandPool m_commandPool;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHISWAPCHAIN_HPP
