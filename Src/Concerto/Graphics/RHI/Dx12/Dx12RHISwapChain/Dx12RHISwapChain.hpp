//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHISWAPCHAIN_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHISWAPCHAIN_HPP

#include "Concerto/Graphics/RHI/SwapChain.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Fence/Fence.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/SwapChain/SwapChain.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandPool/Dx12RHICommandPool.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFrameBuffer/Dx12RHIFrameBuffer.hpp"

namespace cct::gfx::rhi
{
	class Dx12RHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHISwapChain : public rhi::SwapChain, public dx12::SwapChain
	{
	public:
		Dx12RHISwapChain(rhi::Dx12RHIDevice& device, Window& window, PixelFormat pixelFormat, PixelFormat depthPixelFormat);

		RenderPass* GetRenderPass() override;
		Vector2u GetExtent() const override;
		UInt32 GetImageCount() const override;
		rhi::Frame& AcquireFrame() override;
		void WaitAll() const override;
		CommandPool& GetCommandPool();
	private:
		class SwapChainFrame : public rhi::Frame
		{
		public:
			SwapChainFrame(Dx12RHISwapChain& owner, UInt32 imageIndex);

			void Present() override;
			rhi::CommandBuffer& GetCommandBuffer() override;
			std::size_t GetCurrentFrameIndex() override;
			rhi::FrameBuffer& GetFrameBuffer() override;
			void Wait() const;

			const dx12::Fence& GetRenderFence() const;

			static constexpr UInt32 InvalidFrameIndex = std::numeric_limits<UInt32>::max();
		private:
			Dx12RHICommandBuffer m_commandBuffer;
			dx12::Fence m_renderFence;
			Dx12RHIFrameBuffer m_frameBuffer;
			Dx12RHISwapChain* m_owner = nullptr;
			UInt32 m_imageIndex = 0;
		};

		void CreateDepthBuffer();

		Dx12RHIDevice* m_rhiDevice;
		UInt64 m_currentFrameIndex;
		std::unique_ptr<RenderPass> m_renderPass;
		std::vector<SwapChainFrame> m_frames;
		Dx12RHICommandPool m_commandPool;

		// Depth buffer shared by all frames
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthBuffer;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle{};
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DX12_DX12RHISWAPCHAIN_HPP
