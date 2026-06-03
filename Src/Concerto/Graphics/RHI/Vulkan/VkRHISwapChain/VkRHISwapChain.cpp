//
// Created by arthur on 15/05/2024.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHISwapChain/VkRHISwapChain.hpp"

#include <array>
#include <format>

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Core/Window/Window.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandBuffer/VkRHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandPool/VkRHICommandPool.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIRenderPass/VkRHIRenderPass.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"
namespace cct::gfx::rhi
{
	VkRHISwapChain::VkRHISwapChain(rhi::VkRHIDevice& device, Window& window, PixelFormat pixelFormat, PixelFormat depthPixelFormat) :
		rhi::SwapChain(pixelFormat, depthPixelFormat),
		vk::SwapChain(device, window, Converters::ToVulkan(window.GetFormat()), Converters::ToVulkan(depthPixelFormat)),
		m_pixelFormat(window.GetFormat()),
		m_depthPixelFormat(depthPixelFormat)
	{
		CreateRenderPass();
		CreateFrameBuffers(device);
		m_commandPool = device.CreateCommandPool(QueueFamily::Graphics, CommandBufferUsage::Primary);
		m_presentQueue = std::make_unique<vk::Queue>(device, device.GetQueueFamilyIndex(vk::Queue::Type::Graphics));
		CreateFrames();
	}

	VkRHISwapChain::~VkRHISwapChain()
	{
		WaitAll();

		if (m_presentQueue)
			m_presentQueue->WaitIdle();

		// Explicitly destroy frame buffers and render pass before base class destructor
		// This ensures ImageViews are not referenced after they're destroyed
		m_frameBuffers.clear();
		m_frames.clear();
		m_renderPass.reset();
		m_commandPool.reset();
		m_presentQueue.reset();
	}

	rhi::RenderPass* VkRHISwapChain::GetRenderPass()
	{
		CCT_ASSERT(m_renderPass, "ConcertoGraphics: Invalid renderpass");
		return m_renderPass.get();
	}

	Vector2u VkRHISwapChain::GetExtent() const
	{
		return Vector2u{vk::SwapChain::GetExtent().width, vk::SwapChain::GetExtent().height};
	}

	UInt32 VkRHISwapChain::GetImageCount() const
	{
		return static_cast<UInt32>(vk::SwapChain::GetImages().size());
	}

	Frame& VkRHISwapChain::AcquireFrame()
	{
		SwapChainFrame& currentFrame = m_frames[m_currentFrameIndex];
		currentFrame.Wait();
		UInt32 nextImageIndex;
		VkResult result = vk::SwapChain::AcquireNextImage(currentFrame.GetPresentSemaphore(), nextImageIndex, nullptr);
		currentFrame.SetNextImageIndex(nextImageIndex);

		if (m_needResize || result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			GetDevice()->WaitIdle();
			vk::SwapChain::Create(*m_device, GetWindow(), Converters::ToVulkan(m_pixelFormat), Converters::ToVulkan(m_depthPixelFormat));
			m_frameBuffers.clear();
			m_frames.clear();
			CreateRenderPass();
			CreateFrames();
			CreateFrameBuffers(GetRHIDevice());
			m_needResize = false;
			vk::SwapChain::AcquireNextImage(currentFrame.GetPresentSemaphore(), nextImageIndex, nullptr);
			m_frames[m_currentFrameIndex].SetNextImageIndex(nextImageIndex);
			return m_frames[m_currentFrameIndex];
		}

		return currentFrame;
	}

	void VkRHISwapChain::WaitAll() const
	{
		for (auto& frame : m_frames)
			frame.GetRenderFence().Wait(std::numeric_limits<UInt64>::max());
	}

	VkRHICommandPool& VkRHISwapChain::GetCommandPool() const
	{
		CCT_ASSERT(m_commandPool, "ConcertoGraphics: Invalid command pool");
		return Cast<VkRHICommandPool&>(*m_commandPool);
	}

	VkRHIDevice& VkRHISwapChain::GetRHIDevice() const
	{
		CCT_ASSERT(vk::SwapChain::GetDevice(), "ConcertoGraphics: Invalid device");
		return Cast<VkRHIDevice&>(*vk::SwapChain::GetDevice());
	}

	vk::Queue& VkRHISwapChain::GetPresentQueue() const
	{
		CCT_ASSERT(m_presentQueue, "ConcertoGraphics: Invalid present queue");
		return *m_presentQueue;
	}

	rhi::FrameBuffer& VkRHISwapChain::GetCurrentFrameBuffer()
	{
		return *m_frameBuffers[m_currentFrameIndex];
	}

	const rhi::FrameBuffer& VkRHISwapChain::GetCurrentFrameBuffer() const
	{
		return *m_frameBuffers[m_currentFrameIndex];
	}

	void VkRHISwapChain::Present(UInt32 imageIndex)
	{
		CCT_AUTO_PROFILER_SCOPE();

		m_lastFrameIndex = m_currentFrameIndex;
		m_currentFrameIndex = (m_currentFrameIndex + 1) % GetImageCount();

		SwapChainFrame& currentFrame = m_frames[m_lastFrameIndex];
		CCT_ASSERT(currentFrame.GetCurrentFrameIndex() != SwapChainFrame::InvalidFrameIndex, "The current frame has an invalid frame index. Did you forgot to call SwapChainFrame::SetNextImageIndex?");

		if (!m_presentQueue->Present(currentFrame.GetRenderSemaphore(), *this, imageIndex))
		{
			switch (m_presentQueue->GetLastResult())
			{
				case VK_ERROR_OUT_OF_DATE_KHR:
				case VK_SUBOPTIMAL_KHR:
				{
					m_needResize = true;
					break;
				}
				default:
				{
					CCT_ASSERT_FALSE("ConcertoGraphics: Present failed VKResult={}", static_cast<int>(m_presentQueue->GetLastResult()));
				}
			}
		}
	}

	void VkRHISwapChain::CreateFrameBuffers(rhi::VkRHIDevice& device)
	{
		CCT_AUTO_PROFILER_SCOPE();

		const std::span<vk::ImageView> imagesViews = vk::SwapChain::GetImageViews();

		m_frameBuffers.reserve(imagesViews.size());
		for (const vk::ImageView& imageView : imagesViews)
		{
			std::vector<std::unique_ptr<rhi::TextureView>> attachments;
			attachments.emplace_back(std::make_unique<VkRHITextureView>(imageView));
			attachments.emplace_back(std::make_unique<VkRHITextureView>(GetDepthImageView()));
			CCT_ASSERT(imageView.Get() != VK_NULL_HANDLE && GetDepthImageView().Get() != VK_NULL_HANDLE, "ConcertoGraphics: iInvalid attachment");

			auto extent = vk::SwapChain::GetExtent();
			auto fb = device.CreateFrameBuffer(extent.width, extent.height, Cast<VkRHIRenderPass&>(*m_renderPass), attachments);
			CCT_ASSERT(fb, "ConcertoGraphics: Could not create frame buffer");

			m_frameBuffers.emplace_back(std::move(fb));
		}
	}

	void VkRHISwapChain::CreateRenderPass()
	{
		CCT_AUTO_PROFILER_SCOPE();

		std::vector<rhi::RenderPass::Attachment> attachment;
		auto& colorAttachment = attachment.emplace_back();
		colorAttachment.pixelFormat = Converters::FromVulkan(vk::SwapChain::GetImageFormat());
		colorAttachment.loadOp = rhi::AttachmentLoadOp::Clear;
		colorAttachment.storeOp = rhi::AttachmentStoreOp::Store;
		colorAttachment.stencilLoadOp = rhi::AttachmentLoadOp::DontCare;
		colorAttachment.stencilStoreOp = rhi::AttachmentStoreOp::DontCare;
		colorAttachment.initialLayout = rhi::ImageLayout::Undefined;
		colorAttachment.finalLayout = rhi::ImageLayout::PresentSrcKhr;

		auto& depthAttachment = attachment.emplace_back();
		depthAttachment.pixelFormat = GetDepthPixelFormat();
		depthAttachment.loadOp = rhi::AttachmentLoadOp::Clear;
		depthAttachment.storeOp = rhi::AttachmentStoreOp::Store;
		depthAttachment.stencilLoadOp = rhi::AttachmentLoadOp::Clear;
		depthAttachment.stencilStoreOp = rhi::AttachmentStoreOp::DontCare;
		depthAttachment.initialLayout = rhi::ImageLayout::Undefined;
		depthAttachment.finalLayout = rhi::ImageLayout::DepthStencilAttachmentOptimal;

		std::vector<rhi::RenderPass::SubPassDescription> subPassDescriptions;

		auto& subPass = subPassDescriptions.emplace_back();
		subPass.colorAttachments.push_back({0, rhi::ImageLayout::ColorAttachmentOptimal});
		subPass.depthStencilAttachment = {1, rhi::ImageLayout::DepthStencilAttachmentOptimal};

		std::vector<rhi::RenderPass::SubPassDependency> subPassDependencies;
		auto& colorDependency = subPassDependencies.emplace_back();
		colorDependency.srcSubPassIndex = rhi::RenderPass::SubPassDependency::ExternalSubPass;
		colorDependency.dstSubPassIndex = 0u;
		colorDependency.srcStageMask = rhi::PipelineStage::ColorAttachmentOutput;
		colorDependency.srcAccessFlags = {};
		colorDependency.dstStageMask = rhi::PipelineStage::ColorAttachmentOutput;
		colorDependency.dstAccessFlags = rhi::MemoryAccess::ColorAttachmentWrite;

		auto& depthDependency = subPassDependencies.emplace_back();
		depthDependency.srcSubPassIndex = rhi::RenderPass::SubPassDependency::ExternalSubPass;
		depthDependency.dstSubPassIndex = 0u;
		depthDependency.srcStageMask = rhi::PipelineStage::EarlyFragmentTests | rhi::PipelineStage::LateFragmentTests;
		depthDependency.srcAccessFlags = {};
		depthDependency.dstStageMask = rhi::PipelineStage::EarlyFragmentTests | rhi::PipelineStage::LateFragmentTests;
		depthDependency.dstAccessFlags = rhi::MemoryAccess::DepthStencilAttachmentWrite;

		m_renderPass = Cast<VkRHIDevice&>(*m_device).CreateRenderPass(attachment, subPassDescriptions, subPassDependencies);
		CCT_ASSERT(m_renderPass && Cast<VkRHIRenderPass&>(*m_renderPass).GetLastResult() == VK_SUCCESS, "ConcertoGraphics: Could not create render pass");
	}

	void VkRHISwapChain::CreateFrames()
	{
		const UInt32 imageCount = GetImageCount();
		if (m_frames.size() != imageCount)
		{
			m_frames.clear();
			m_frames.reserve(imageCount);
			for (UInt32 i = 0; i < imageCount; ++i)
			{
				m_frames.emplace_back(*this);
			}
		}
	}

	VkRHISwapChain::SwapChainFrame::SwapChainFrame(VkRHISwapChain& owner) :
		m_commandBuffer(owner.GetCommandPool().AllocateCommandBuffer()),
		m_renderFence(*owner.GetDevice()),
		m_presentSemaphore(*owner.GetDevice()),
		m_renderSemaphore(*owner.GetDevice()),
		m_owner(&owner),
		m_imageIndex(InvalidFrameIndex)
	{
#ifdef CCT_ENABLE_OBJECT_DEBUG
		Cast<VkRHICommandBuffer&>(*m_commandBuffer).SetDebugName("SwapChainFrameCommandBuffer");
		m_renderFence.SetDebugName("SwapChainFrameRenderFence");
		m_presentSemaphore.SetDebugName("SwapChainFramePresentSemaphore");
		m_renderSemaphore.SetDebugName("SwapChainFrameRenderSemaphore");
#endif
	}

	void VkRHISwapChain::SwapChainFrame::Present()
	{
		CCT_AUTO_PROFILER_SCOPE();

		const vk::Queue& presentQueue = m_owner->GetPresentQueue();

		m_renderFence.Reset();
		presentQueue.Submit(Cast<VkRHICommandBuffer&>(*m_commandBuffer), &m_presentSemaphore, &m_renderSemaphore, m_renderFence);
		m_owner->Present(m_imageIndex);
		CCT_FRAME_MARK();
	}

	rhi::CommandBuffer& VkRHISwapChain::SwapChainFrame::GetCommandBuffer()
	{
		return *m_commandBuffer;
	}

	std::size_t VkRHISwapChain::SwapChainFrame::GetCurrentFrameIndex()
	{
		return m_imageIndex;
	}

	rhi::FrameBuffer& VkRHISwapChain::SwapChainFrame::GetFrameBuffer()
	{
		return m_owner->GetCurrentFrameBuffer();
	}

	void VkRHISwapChain::SwapChainFrame::SetNextImageIndex(UInt32 imageIndex)
	{
		m_imageIndex = imageIndex;
#ifdef CCT_ENABLE_OBJECT_DEBUG
		Cast<VkRHICommandBuffer&>(*m_commandBuffer).SetDebugName(std::format("SwapChainFrameCommandBuffer[{}]", imageIndex));
		m_renderFence.SetDebugName(std::format("SwapChainFrameRenderFence {}", imageIndex));
		m_presentSemaphore.SetDebugName(std::format("SwapChainFramePresentSemaphore {}", imageIndex));
		m_renderSemaphore.SetDebugName(std::format("SwapChainFrameRenderSemaphore {}", imageIndex));
#endif
	}

	void VkRHISwapChain::SwapChainFrame::Wait() const
	{
		CCT_AUTO_PROFILER_SCOPE();

		m_renderFence.Wait(-1);
		m_renderFence.Reset();
	}

	const vk::Semaphore& VkRHISwapChain::SwapChainFrame::GetPresentSemaphore() const
	{
		return m_presentSemaphore;
	}

	const vk::Semaphore& VkRHISwapChain::SwapChainFrame::GetRenderSemaphore() const
	{
		return m_renderSemaphore;
	}

	const vk::Fence& VkRHISwapChain::SwapChainFrame::GetRenderFence() const
	{
		return m_renderFence;
	}
} // namespace cct::gfx::rhi
