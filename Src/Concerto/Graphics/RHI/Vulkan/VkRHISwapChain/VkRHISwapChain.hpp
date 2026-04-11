//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_SWAPCHAIN_HPP
#define CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_SWAPCHAIN_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Fence/Fence.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/RenderPass/RenderPass.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Semaphore/Semaphore.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/SwapChain/SwapChain.hpp"

#include "Concerto/Graphics/RHI/SwapChain.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandPool/VkRHICommandPool.hpp"

namespace cct::gfx::vk
{
	class Queue;
	class CommandPool;
}

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHISwapChain final : public rhi::SwapChain, public vk::SwapChain
	{
	public:
		VkRHISwapChain(rhi::VkRHIDevice& device, Window& window, PixelFormat pixelFormat, PixelFormat depthPixelFormat);
		~VkRHISwapChain() override;

		rhi::RenderPass* GetRenderPass() override;
		Vector2u GetExtent() const override;
		UInt32 GetImageCount() const override;
		Frame& AcquireFrame() override;
		void WaitAll() const override;

		inline VkRHICommandPool& GetCommandPool() const;
		inline VkRHIDevice& GetRHIDevice() const;
		inline vk::Queue& GetPresentQueue() const;
		inline rhi::FrameBuffer& GetCurrentFrameBuffer();
		inline const rhi::FrameBuffer& GetCurrentFrameBuffer() const;

		void Present(UInt32 imageIndex);
	private:
		void CreateFrameBuffers(rhi::VkRHIDevice& device);
		void CreateRenderPass();
		void CreateFrames();

		class SwapChainFrame : public rhi::Frame
		{
		public:
			SwapChainFrame(VkRHISwapChain& owner);

			void Present() override;
			rhi::CommandBuffer& GetCommandBuffer() override;
			std::size_t GetCurrentFrameIndex() override;
			rhi::FrameBuffer& GetFrameBuffer() override;
			void SetNextImageIndex(UInt32 imageIndex);
			void Wait() const;

			const vk::Semaphore& GetPresentSemaphore() const;
			const vk::Semaphore& GetRenderSemaphore() const;
			const vk::Fence& GetRenderFence() const;

			static constexpr UInt32 InvalidFrameIndex = std::numeric_limits<UInt32>::max();
		private:
			std::unique_ptr<rhi::CommandBuffer> m_commandBuffer;
			vk::Fence m_renderFence;
			vk::Semaphore m_presentSemaphore;
			vk::Semaphore m_renderSemaphore;
			VkRHISwapChain* m_owner = nullptr;
			UInt32 m_imageIndex = 0;
		};

		PixelFormat m_pixelFormat;
		PixelFormat m_depthPixelFormat;

		std::vector<std::unique_ptr<FrameBuffer>> m_frameBuffers;
		std::unique_ptr<rhi::RenderPass> m_renderPass;
		std::unique_ptr<rhi::CommandPool> m_commandPool;
		std::unique_ptr<vk::Queue> m_presentQueue;
		std::vector<SwapChainFrame> m_frames;

		UInt32 m_currentFrameIndex = 0;
		UInt32 m_lastFrameIndex = 0;
		bool m_needResize = false;
	};
}

#endif //CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_SWAPCHAIN_HPP