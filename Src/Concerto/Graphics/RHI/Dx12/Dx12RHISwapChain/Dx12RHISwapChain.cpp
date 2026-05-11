//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHISwapChain/Dx12RHISwapChain.hpp"

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandList/CommandList.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

namespace cct::gfx::rhi
{
	Dx12RHISwapChain::Dx12RHISwapChain(rhi::Dx12RHIDevice& device, Window& window, PixelFormat pixelFormat, PixelFormat depthPixelFormat) :
		rhi::SwapChain(pixelFormat, depthPixelFormat),
		dx12::SwapChain(device, window),
		m_rhiDevice(&device),
		m_renderPass(std::make_unique<RenderPass>()),
		m_currentFrameIndex(0),
		m_commandPool(device, CommandBufferUsage::Primary, D3D12_COMMAND_LIST_TYPE_DIRECT)
	{
		// Register the swapchain's command queue on the device for upload operations
		device.RegisterRenderQueue(GetCommandQueue().Get());

		CreateDepthBuffer();

		m_frames.reserve(dx12::SwapChain::ImageCount);
		for (UINT32 i = 0; i < dx12::SwapChain::ImageCount; ++i)
			m_frames.emplace_back(*this, i);
	}

	void Dx12RHISwapChain::CreateDepthBuffer()
	{
		auto extent = GetExtent();
		auto* d3dDevice = m_rhiDevice->dx12::Device::Get();

		// Create DSV descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));

		m_dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

		// Create depth buffer resource
		D3D12_RESOURCE_DESC depthDesc{};
		depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthDesc.Width = extent.X();
		depthDesc.Height = extent.Y();
		depthDesc.DepthOrArraySize = 1;
		depthDesc.MipLevels = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue{};
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		d3dDevice->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE,
			&depthDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue, IID_PPV_ARGS(&m_depthBuffer));

		// Create DSV
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		d3dDevice->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, m_dsvHandle);
	}

	RenderPass* Dx12RHISwapChain::GetRenderPass()
	{
		return m_renderPass.get();
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
		m_commandBuffer(Cast<Dx12RHICommandPool&>(owner.GetCommandPool()), D3D12_COMMAND_LIST_TYPE_DIRECT, *owner.m_rhiDevice),
		m_frameBuffer(
			owner.GetExtent().X(), owner.GetExtent().Y(),
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>{owner.GetRenderTargetViewHandles()[imageIndex]},
			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>{owner.GetRenderTargets()[imageIndex]},
			owner.m_dsvHandle,
			owner.m_depthBuffer)
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

	rhi::FrameBuffer& Dx12RHISwapChain::SwapChainFrame::GetFrameBuffer()
	{
		return m_frameBuffer;
	}

	void Dx12RHISwapChain::SwapChainFrame::Wait() const
	{
		m_owner->GetCommandQueue()->Signal(m_renderFence.Get(), m_renderFence.GetCompletedValue());
		m_renderFence.Wait();
	}

	const dx12::Fence& Dx12RHISwapChain::SwapChainFrame::GetRenderFence() const
	{
		return m_renderFence;
	}
} // namespace cct::gfx::rhi
