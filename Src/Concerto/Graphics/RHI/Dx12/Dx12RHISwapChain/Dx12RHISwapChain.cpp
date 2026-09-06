//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHISwapChain/Dx12RHISwapChain.hpp"

#include <Concerto/Core/Assert.hpp>
#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandList/CommandList.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Factory/Factory.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHITexture/Dx12RHITexture.hpp"

namespace cct::gfx::rhi
{
	Dx12RHISwapChain::Dx12RHISwapChain(rhi::Dx12RHIDevice& device, Window& window, PixelFormat pixelFormat, PixelFormat depthPixelFormat) :
		rhi::SwapChain(pixelFormat, depthPixelFormat),
		dx12::SwapChain(device, window),
		m_rhiDevice(&device),
		m_currentFrameIndex(0),
		m_commandPool(device, CommandBufferUsage::Primary, D3D12_COMMAND_LIST_TYPE_DIRECT)
	{
		// Register the swapchain's command queue on the device for upload operations
		device.RegisterRenderQueue(GetCommandQueue().Get());

		CreateColorTextures();

		m_frames.reserve(dx12::SwapChain::ImageCount);
		for (UINT32 i = 0; i < dx12::SwapChain::ImageCount; ++i)
			m_frames.emplace_back(*this, i);
	}

	void Dx12RHISwapChain::CreateColorTextures()
	{
		const Vector2u extent = GetExtent();
		const DXGI_FORMAT format = dx12::Factory::PixelFormatToDXGI(GetPixelFormat());

		m_colorTextures.clear();
		m_colorTextures.reserve(dx12::SwapChain::ImageCount);
		for (UInt32 i = 0; i < dx12::SwapChain::ImageCount; ++i)
		{
			m_colorTextures.push_back(std::make_shared<Dx12RHITexture>(
				*m_rhiDevice, GetRenderTargets()[i], format, extent.X(), extent.Y()));
		}
	}

	std::shared_ptr<Texture> Dx12RHISwapChain::GetColorTexture(UInt32 imageIndex)
	{
		CCT_ASSERT(imageIndex < m_colorTextures.size(), "ConcertoGraphics: Invalid swapchain image index {}", imageIndex);
		return m_colorTextures[imageIndex];
	}

	Vector2u Dx12RHISwapChain::GetExtent() const
	{
		return dx12::SwapChain::GetExtent();
	}

	UInt32 Dx12RHISwapChain::GetImageCount() const
	{
		return dx12::SwapChain::ImageCount;
	}

	rhi::Frame& Dx12RHISwapChain::AcquireFrame()
	{
		// Use the DXGI swapchain's actual current back buffer index
		m_currentFrameIndex = dx12::SwapChain::Get()->GetCurrentBackBufferIndex();
		m_frames[m_currentFrameIndex].Wait();
		return m_frames[m_currentFrameIndex];
	}

	void Dx12RHISwapChain::WaitAll() const
	{
		for (auto& frame : m_frames)
			frame.GetRenderFence().Wait();
	}

	CommandPool& Dx12RHISwapChain::GetCommandPool()
	{
		return m_commandPool;
	}

	Dx12RHISwapChain::SwapChainFrame::SwapChainFrame(Dx12RHISwapChain& owner, UINT32 imageIndex) :
		m_renderFence(*owner.GetDevice()),
		m_owner(&owner),
		m_imageIndex(imageIndex),
		m_commandBuffer(Cast<Dx12RHICommandPool&>(owner.GetCommandPool()), D3D12_COMMAND_LIST_TYPE_DIRECT, *owner.m_rhiDevice)
	{
	}

	void Dx12RHISwapChain::SwapChainFrame::Present()
	{
		std::array<ID3D12CommandList*, 1> commandLists = {
			m_commandBuffer.Get()};
		m_owner->GetCommandQueue()->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
		m_owner->dx12::SwapChain::Get()->Present(1, 0);
		CCT_FRAME_MARK();
	}

	rhi::CommandBuffer& Dx12RHISwapChain::SwapChainFrame::GetCommandBuffer()
	{
		return m_commandBuffer;
	}

	std::size_t Dx12RHISwapChain::SwapChainFrame::GetCurrentFrameIndex()
	{
		return m_imageIndex;
	}

	void Dx12RHISwapChain::SwapChainFrame::Wait() const
	{
		m_renderFence.Signal(*m_owner->GetCommandQueue().Get());
		m_renderFence.Wait();
	}

	const dx12::Fence& Dx12RHISwapChain::SwapChainFrame::GetRenderFence() const
	{
		return m_renderFence;
	}
} // namespace cct::gfx::rhi
