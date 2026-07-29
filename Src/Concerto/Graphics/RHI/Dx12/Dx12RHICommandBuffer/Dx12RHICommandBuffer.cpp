//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandAllocator/CommandAllocator.hpp"
#include "Concerto/Graphics/Core/Vertex.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIBuffer/Dx12RHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSet/Dx12RHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFrameBuffer/Dx12RHIFrameBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIPipeline/Dx12RHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIPipelineLayout/Dx12RHIPipelineLayout.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHITexture/Dx12RHITexture.hpp"
#include "Concerto/Graphics/RHI/Material.hpp"

namespace cct::gfx::rhi
{
	Dx12RHICommandBuffer::Dx12RHICommandBuffer(dx12::CommandAllocator& owner, D3D12_COMMAND_LIST_TYPE type, Dx12RHIDevice& device) :
		rhi::CommandBuffer(),
		dx12::CommandList(owner, type),
		m_device(&device)
	{
	}

	void Dx12RHICommandBuffer::Begin()
	{
		dx12::CommandList::Reset();
	}

	void Dx12RHICommandBuffer::End()
	{
		HRESULT [[maybe_unused]] result = Close();
	}

	void Dx12RHICommandBuffer::Reset()
	{
		// Handled in Begin() via dx12::CommandList::Reset()
	}

	void Dx12RHICommandBuffer::SetViewport(const Viewport& viewport)
	{
		if (!IsValid())
			return;

		D3D12_VIEWPORT d3dViewport = {};
		d3dViewport.TopLeftX = viewport.x;
		d3dViewport.TopLeftY = viewport.y;
		d3dViewport.Width = viewport.width;
		d3dViewport.Height = viewport.height;
		d3dViewport.MinDepth = viewport.minDepth;
		d3dViewport.MaxDepth = viewport.maxDepth;

		Get()->RSSetViewports(1, &d3dViewport);
	}

	void Dx12RHICommandBuffer::SetScissor(const Rect2D& scissor)
	{
		if (!IsValid())
			return;

		D3D12_RECT d3dRect = {};
		d3dRect.left = scissor.x;
		d3dRect.top = scissor.y;
		d3dRect.right = scissor.x + static_cast<LONG>(scissor.width);
		d3dRect.bottom = scissor.y + static_cast<LONG>(scissor.height);

		Get()->RSSetScissorRects(1, &d3dRect);
	}

	void Dx12RHICommandBuffer::BeginRenderPass(const rhi::RenderPass& renderPass, const rhi::FrameBuffer& frameBuffer, const Vector3f& clearColor)
	{
		if (!IsValid())
			return;

		const auto& dx12FrameBuffer = Cast<const Dx12RHIFrameBuffer&>(frameBuffer);
		m_currentFrameBuffer = &dx12FrameBuffer;

		// Transition render targets from PRESENT to RENDER_TARGET
		for (const auto& rtResource : dx12FrameBuffer.GetRenderTargetResources())
		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = rtResource.Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			Get()->ResourceBarrier(1, &barrier);
		}

		// Set render targets
		const auto& rtvHandles = dx12FrameBuffer.GetRTVHandles();
		const auto& dsvHandle = dx12FrameBuffer.GetDSVHandle();

		if (!rtvHandles.empty())
		{
			Get()->OMSetRenderTargets(
				static_cast<UINT>(rtvHandles.size()),
				rtvHandles.data(),
				FALSE,
				dsvHandle.has_value() ? &dsvHandle.value() : nullptr);
		}

		// Clear render targets
		float clearColorArray[4] = {clearColor.X(), clearColor.Y(), clearColor.Z(), 1.0f};
		for (const auto& rtvHandle : rtvHandles)
		{
			Get()->ClearRenderTargetView(rtvHandle, clearColorArray, 0, nullptr);
		}

		// Clear depth stencil
		if (dsvHandle.has_value())
		{
			Get()->ClearDepthStencilView(dsvHandle.value(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		}
	}

	void Dx12RHICommandBuffer::EndRenderPass()
	{
		if (!IsValid() || !m_currentFrameBuffer)
			return;

		// Transition render targets from RENDER_TARGET to PRESENT
		for (const auto& rtResource : m_currentFrameBuffer->GetRenderTargetResources())
		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = rtResource.Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			Get()->ResourceBarrier(1, &barrier);
		}

		m_currentFrameBuffer = nullptr;
	}

	void Dx12RHICommandBuffer::BindMaterial(const Material& material)
	{
		if (!IsValid() || !m_device)
			return;

		// Set pipeline state
		if (auto* dx12Pipeline = dynamic_cast<const Dx12RHIPipeline*>(material.pipeline.get()))
		{
			Get()->SetPipelineState(dx12Pipeline->GetPipelineState());
			Get()->SetGraphicsRootSignature(dx12Pipeline->GetLayout().GetRootSignature().Get());

			// Set descriptor heaps before binding descriptor tables
			auto& pool = m_device->GetDescriptorPool();
			ID3D12DescriptorHeap* heaps[] = {
				pool.GetGpuHeap()->GetHeap(),
				pool.GetSamplerHeap()->GetHeap()};
			Get()->SetDescriptorHeaps(_countof(heaps), heaps);

			// Bind descriptor sets
			for (std::size_t i = 0; i < material.descriptorSets.size(); ++i)
			{
				if (auto* dx12DescSet = dynamic_cast<const Dx12RHIDescriptorSet*>(material.descriptorSets[i].get()))
				{
					const auto& rootSig = dx12Pipeline->GetLayout().GetRootSignature();

					// Bind CBV/SRV/UAV table
					const auto& gpuRange = dx12DescSet->GetDescriptorRange();
					if (gpuRange.baseHandle.IsValid())
					{
						UINT rootParamIndex = rootSig.GetRootParameterIndex(static_cast<UINT>(i), false);
						if (rootParamIndex != UINT_MAX)
							Get()->SetGraphicsRootDescriptorTable(rootParamIndex, gpuRange.baseHandle.gpuHandle);
					}

					// Bind sampler table
					if (dx12DescSet->HasSamplers())
					{
						const auto& samplerRange = dx12DescSet->GetSamplerRange();
						UINT samplerRootParam = rootSig.GetRootParameterIndex(static_cast<UINT>(i), true);
						if (samplerRootParam != UINT_MAX)
							Get()->SetGraphicsRootDescriptorTable(samplerRootParam, samplerRange.baseHandle.gpuHandle);
					}
				}
			}
		}
	}

	void Dx12RHICommandBuffer::BindVertexBuffer(const rhi::Buffer& buffer)
	{
		if (!IsValid())
			return;

		const auto& dx12Buffer = Cast<const Dx12RHIBuffer&>(buffer);

		D3D12_VERTEX_BUFFER_VIEW view = {};
		view.BufferLocation = dx12Buffer.GetGPUVirtualAddress();
		view.SizeInBytes = dx12Buffer.GetSize();
		view.StrideInBytes = sizeof(cct::gfx::Vertex);

		Get()->IASetVertexBuffers(0, 1, &view);
		Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Dx12RHICommandBuffer::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		if (!IsValid())
			return;

		Get()->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void Dx12RHICommandBuffer::Copy(const Buffer& src, const Texture& dst)
	{
		if (!IsValid() || !m_device)
			return;

		const auto& dx12Buffer = Cast<const Dx12RHIBuffer&>(src);
		const auto& dx12Texture = Cast<const Dx12RHITexture&>(dst);
		auto* d3dDevice = m_device->dx12::Device::Get();

		D3D12_RESOURCE_DESC texDesc = dx12Texture.GetResource()->GetDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
		UINT numRows = 0;
		UINT64 rowSizeInBytes = 0;
		UINT64 totalBytes = 0;
		d3dDevice->GetCopyableFootprints(&texDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

		D3D12_TEXTURE_COPY_LOCATION dstLoc{};
		dstLoc.pResource = dx12Texture.GetResource();
		dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLoc.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION srcLoc{};
		srcLoc.pResource = dx12Buffer.GetResource();
		srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLoc.PlacedFootprint = footprint;

		Get()->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);
	}

	void Dx12RHICommandBuffer::Copy(const Texture& src, const Buffer& dst, UInt64 dstOffset)
	{
		if (!IsValid() || !m_device)
			return;

		const auto& dx12Texture = Cast<const Dx12RHITexture&>(src);
		const auto& dx12Buffer = Cast<const Dx12RHIBuffer&>(dst);
		auto* d3dDevice = m_device->dx12::Device::Get();

		D3D12_RESOURCE_DESC texDesc = dx12Texture.GetResource()->GetDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
		UINT numRows = 0;
		UINT64 rowSizeInBytes = 0;
		UINT64 totalBytes = 0;
		d3dDevice->GetCopyableFootprints(&texDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

		D3D12_TEXTURE_COPY_LOCATION srcLoc{};
		srcLoc.pResource = dx12Texture.GetResource();
		srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		srcLoc.SubresourceIndex = 0;

		footprint.Offset = dstOffset;

		D3D12_TEXTURE_COPY_LOCATION dstLoc{};
		dstLoc.pResource = dx12Buffer.GetResource();
		dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		dstLoc.PlacedFootprint = footprint;

		Get()->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);
	}

	void Dx12RHICommandBuffer::TransitionImageLayout(const Texture& texture, ImageLayout oldLayout, ImageLayout newLayout)
	{
		if (!IsValid())
			return;

		const auto& dx12Texture = Cast<const Dx12RHITexture&>(texture);

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = dx12Texture.GetResource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = ToD3D12ResourceState(oldLayout);
		barrier.Transition.StateAfter = ToD3D12ResourceState(newLayout);

		Get()->ResourceBarrier(1, &barrier);
	}

	D3D12_RESOURCE_STATES Dx12RHICommandBuffer::ToD3D12ResourceState(ImageLayout layout)
	{
		switch (layout)
		{
			case ImageLayout::Undefined:
				return D3D12_RESOURCE_STATE_COMMON;
			case ImageLayout::General:
				return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			case ImageLayout::ColorAttachmentOptimal:
				return D3D12_RESOURCE_STATE_RENDER_TARGET;
			case ImageLayout::DepthStencilAttachmentOptimal:
				return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			case ImageLayout::DepthStencilReadOnlyOptimal:
				return D3D12_RESOURCE_STATE_DEPTH_READ;
			case ImageLayout::ShaderReadOnlyOptimal:
				return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			case ImageLayout::TransferSrcOptimal:
				return D3D12_RESOURCE_STATE_COPY_SOURCE;
			case ImageLayout::TransferDstOptimal:
				return D3D12_RESOURCE_STATE_COPY_DEST;
			case ImageLayout::PresentSrcKhr:
				return D3D12_RESOURCE_STATE_PRESENT;
			default:
				return D3D12_RESOURCE_STATE_COMMON;
		}
	}

	void Dx12RHICommandBuffer::ExecuteCommands(std::span<CommandBuffer*> /*secondaryCmdBufs*/)
	{
		// TODO: DX12 bundle support is restricted (no transfers/barriers).
		// Secondary CB commands must be inlined into the primary for DX12.
	}
	void Dx12RHICommandBuffer::PipelineBarrier(const Texture& texture,
											   ImageLayout oldLayout,
											   ImageLayout newLayout,
											   PipelineStageFlags /*srcStage*/,
											   PipelineStageFlags /*dstStage*/,
											   MemoryAccessFlags /*srcAccess*/,
											   MemoryAccessFlags /*dstAccess*/)
	{
		// DX12 encodes all stage/access information in resource states.
		// Delegate directly to TransitionImageLayout which handles the D3D12_RESOURCE_BARRIER.
		TransitionImageLayout(texture, oldLayout, newLayout);
	}

	void Dx12RHICommandBuffer::PipelineBarrier(const Buffer& buffer,
											   PipelineStageFlags /*srcStage*/,
											   PipelineStageFlags /*dstStage*/,
											   MemoryAccessFlags /*srcAccess*/,
											   MemoryAccessFlags /*dstAccess*/)
	{
		if (!IsValid())
			return;

		const auto& dx12Buffer = Cast<const Dx12RHIBuffer&>(buffer);

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.UAV.pResource = dx12Buffer.GetResource();

		Get()->ResourceBarrier(1, &barrier);
	}

	void Dx12RHICommandBuffer::BindComputePipeline(const Pipeline& pipeline)
	{
		if (!IsValid() || !m_device)
			return;

		const auto& dx12Pipeline = Cast<const Dx12RHIPipeline&>(pipeline);
		Get()->SetPipelineState(dx12Pipeline.GetPipelineState());
		Get()->SetComputeRootSignature(dx12Pipeline.GetLayout().GetRootSignature().Get());

		auto& pool = m_device->GetDescriptorPool();
		ID3D12DescriptorHeap* heaps[] = {
			pool.GetGpuHeap()->GetHeap(),
			pool.GetSamplerHeap()->GetHeap()};
		Get()->SetDescriptorHeaps(_countof(heaps), heaps);
	}

	void Dx12RHICommandBuffer::BindComputeDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set)
	{
		if (!IsValid())
			return;

		const auto& dx12Layout = Cast<const Dx12RHIPipelineLayout&>(layout);
		const auto& dx12Set = Cast<const Dx12RHIDescriptorSet&>(set);
		const auto& rootSig = dx12Layout.GetRootSignature();

		const auto& gpuRange = dx12Set.GetDescriptorRange();
		if (gpuRange.baseHandle.IsValid())
		{
			UINT rootParamIndex = rootSig.GetRootParameterIndex(0, false);
			if (rootParamIndex != UINT_MAX)
				Get()->SetComputeRootDescriptorTable(rootParamIndex, gpuRange.baseHandle.gpuHandle);
		}

		if (dx12Set.HasSamplers())
		{
			const auto& samplerRange = dx12Set.GetSamplerRange();
			UINT samplerRootParam = rootSig.GetRootParameterIndex(0, true);
			if (samplerRootParam != UINT_MAX)
				Get()->SetComputeRootDescriptorTable(samplerRootParam, samplerRange.baseHandle.gpuHandle);
		}
	}

	void Dx12RHICommandBuffer::Dispatch(UInt32 groupCountX, UInt32 groupCountY, UInt32 groupCountZ)
	{
		if (!IsValid())
			return;

		Get()->Dispatch(groupCountX, groupCountY, groupCountZ);
	}
} // namespace cct::gfx::rhi
