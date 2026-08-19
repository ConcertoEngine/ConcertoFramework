//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

#include <Concerto/Core/Assert.hpp>
#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandAllocator/CommandAllocator.hpp"
#include "Concerto/Graphics/Core/Vertex.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIBuffer/Dx12RHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSet/Dx12RHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFrameBuffer/Dx12RHIFrameBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIPipeline/Dx12RHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIPipelineLayout/Dx12RHIPipelineLayout.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIRenderPass/Dx12RHIRenderPass.hpp"
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

		if (dx12FrameBuffer.IsSwapchainTarget())
		{
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
		}

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

		// Mirror Vulkan: vkCmdBeginRenderPass only clears attachments whose loadOp
		// is Clear. Always-clearing here wiped Load/DontCare targets (ping-pong).
		std::vector<AttachmentLoadOp> colorLoadOps;
		AttachmentLoadOp depthLoadOp = AttachmentLoadOp::Clear;
		if (const auto* dx12Pass = dynamic_cast<const Dx12RHIRenderPass*>(&renderPass))
		{
			depthLoadOp = AttachmentLoadOp::DontCare;
			for (const auto& attachment : dx12Pass->GetAttachments())
			{
				if (attachment.finalLayout == ImageLayout::DepthStencilAttachmentOptimal)
					depthLoadOp = attachment.loadOp;
				else
					colorLoadOps.push_back(attachment.loadOp);
			}
		}

		const float clearColorArray[4] = {clearColor.X(), clearColor.Y(), clearColor.Z(), 0.f};
		for (std::size_t i = 0; i < rtvHandles.size(); ++i)
		{
			const bool shouldClear = i >= colorLoadOps.size() || colorLoadOps[i] == AttachmentLoadOp::Clear;
			if (shouldClear)
				Get()->ClearRenderTargetView(rtvHandles[i], clearColorArray, 0, nullptr);
		}

		if (dsvHandle.has_value() && depthLoadOp == AttachmentLoadOp::Clear)
			Get()->ClearDepthStencilView(dsvHandle.value(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	}

	void Dx12RHICommandBuffer::EndRenderPass()
	{
		if (!IsValid() || !m_currentFrameBuffer)
			return;

		// See the matching comment in BeginRenderPass.
		if (m_currentFrameBuffer->IsSwapchainTarget())
		{
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
		}

		m_currentFrameBuffer = nullptr;
	}

	void Dx12RHICommandBuffer::SetHeapsAndRootSignature(const Dx12RHIPipeline& pipeline, bool isCompute)
	{
		Get()->SetPipelineState(pipeline.GetPipelineState());
		if (isCompute)
			Get()->SetComputeRootSignature(pipeline.GetLayout().GetRootSignature().Get());
		else
		{
			Get()->SetGraphicsRootSignature(pipeline.GetLayout().GetRootSignature().Get());
			m_currentVertexStride = pipeline.GetVertexStride();
		}

		auto& pool = m_device->GetDescriptorPool();
		ID3D12DescriptorHeap* heaps[] = {
			pool.GetGpuHeap()->GetHeap(),
			pool.GetSamplerHeap()->GetHeap()};
		Get()->SetDescriptorHeaps(_countof(heaps), heaps);
	}

	void Dx12RHICommandBuffer::BindDescriptorSetImpl(const dx12::Dx12RootSignature& rootSig, const Dx12RHIDescriptorSet& set, UINT setIndex, bool isCompute)
	{
		const auto& gpuRange = set.GetDescriptorRange();
		if (gpuRange.baseHandle.IsValid())
		{
			UINT rootParamIndex = rootSig.GetRootParameterIndex(setIndex, false);
			if (rootParamIndex != UINT_MAX)
			{
				if (isCompute)
					Get()->SetComputeRootDescriptorTable(rootParamIndex, gpuRange.baseHandle.gpuHandle);
				else
					Get()->SetGraphicsRootDescriptorTable(rootParamIndex, gpuRange.baseHandle.gpuHandle);
			}
		}

		if (set.HasSamplers())
		{
			const auto& samplerRange = set.GetSamplerRange();
			UINT samplerRootParam = rootSig.GetRootParameterIndex(setIndex, true);
			if (samplerRootParam != UINT_MAX)
			{
				if (isCompute)
					Get()->SetComputeRootDescriptorTable(samplerRootParam, samplerRange.baseHandle.gpuHandle);
				else
					Get()->SetGraphicsRootDescriptorTable(samplerRootParam, samplerRange.baseHandle.gpuHandle);
			}
		}
	}

	void Dx12RHICommandBuffer::BindMaterial(const Material& material)
	{
		if (!IsValid() || !m_device)
			return;

		auto* dx12Pipeline = dynamic_cast<const Dx12RHIPipeline*>(material.pipeline.get());
		if (!dx12Pipeline)
			return;

		SetHeapsAndRootSignature(*dx12Pipeline, /* isCompute */ false);

		const auto& rootSig = dx12Pipeline->GetLayout().GetRootSignature();
		for (std::size_t i = 0; i < material.descriptorSets.size(); ++i)
		{
			if (auto* dx12DescSet = dynamic_cast<const Dx12RHIDescriptorSet*>(material.descriptorSets[i].get()))
				BindDescriptorSetImpl(rootSig, *dx12DescSet, static_cast<UINT>(i), /* isCompute */ false);
		}
	}

	void Dx12RHICommandBuffer::BindPipeline(const Pipeline& pipeline)
	{
		if (!IsValid() || !m_device)
			return;

		SetHeapsAndRootSignature(Cast<const Dx12RHIPipeline&>(pipeline), /* isCompute */ false);
	}

	void Dx12RHICommandBuffer::BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set)
	{
		if (!IsValid())
			return;

		const auto& dx12Layout = Cast<const Dx12RHIPipelineLayout&>(layout);
		const auto& dx12Set = Cast<const Dx12RHIDescriptorSet&>(set);
		BindDescriptorSetImpl(dx12Layout.GetRootSignature(), dx12Set, 0, /* isCompute */ false);
	}

	void Dx12RHICommandBuffer::BindDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set,
												 UInt32 /*dynamicOffset*/)
	{
		BindDescriptorSet(layout, set);
	}

	void Dx12RHICommandBuffer::BindIndexBuffer(const Buffer& buffer, bool use32bitIndices)
	{
		if (!IsValid())
			return;

		const auto& dx12Buffer = Cast<const Dx12RHIBuffer&>(buffer);
		D3D12_INDEX_BUFFER_VIEW view{};
		view.BufferLocation = dx12Buffer.GetGPUVirtualAddress();
		view.SizeInBytes = dx12Buffer.GetSize();
		view.Format = use32bitIndices ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		Get()->IASetIndexBuffer(&view);
	}

	void Dx12RHICommandBuffer::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex,
										   Int32 vertexOffset, UInt32 firstInstance)
	{
		if (!IsValid())
			return;
		Get()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void Dx12RHICommandBuffer::ClearTexture(const Texture& texture, const Vector4f& clearColor)
	{
		if (!IsValid())
			return;

		const auto& dx12Texture = Cast<const Dx12RHITexture&>(texture);
		const D3D12_CPU_DESCRIPTOR_HANDLE rtv = dx12Texture.GetRTVHandle();
		if (rtv.ptr == 0)
			return;

		TransitionImageLayout(texture, ImageLayout::Undefined, ImageLayout::ColorAttachmentOptimal);
		const float color[4] = {clearColor.X(), clearColor.Y(), clearColor.Z(), clearColor[3]};
		Get()->ClearRenderTargetView(rtv, color, 0, nullptr);
	}

	void Dx12RHICommandBuffer::BindVertexBuffer(const rhi::Buffer& buffer)
	{
		if (!IsValid())
			return;

		const auto& dx12Buffer = Cast<const Dx12RHIBuffer&>(buffer);

		D3D12_VERTEX_BUFFER_VIEW view = {};
		view.BufferLocation = dx12Buffer.GetGPUVirtualAddress();
		view.SizeInBytes = dx12Buffer.GetSize();
		// Falls back to cct::gfx::Vertex's stride if nothing has bound a pipeline yet.
		view.StrideInBytes = m_currentVertexStride != 0 ? m_currentVertexStride : static_cast<UInt32>(sizeof(cct::gfx::Vertex));

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

		const D3D12_RESOURCE_STATES bufState = dx12Buffer.GetState();
		if (bufState != D3D12_RESOURCE_STATE_COPY_SOURCE &&
			bufState != D3D12_RESOURCE_STATE_GENERIC_READ)
		{
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = dx12Buffer.GetResource();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = bufState;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
			Get()->ResourceBarrier(1, &barrier);
			dx12Buffer.SetState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		}

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

		if (dstOffset + totalBytes > dx12Buffer.GetSize())
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Texture→buffer copy overflows dest (offset={} bytes={} dest={})",
							 dstOffset, totalBytes, dx12Buffer.GetSize());
			return;
		}

		const D3D12_RESOURCE_STATES bufState = dx12Buffer.GetState();
		if (bufState == D3D12_RESOURCE_STATE_GENERIC_READ)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Texture→buffer copy dest is an UPLOAD heap; create with HostReadback");
			return;
		}
		if (bufState != D3D12_RESOURCE_STATE_COPY_DEST)
		{
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = dx12Buffer.GetResource();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = bufState;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			Get()->ResourceBarrier(1, &barrier);
			dx12Buffer.SetState(D3D12_RESOURCE_STATE_COPY_DEST);
		}

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

	void Dx12RHICommandBuffer::TransitionImageLayout(const Texture& texture, ImageLayout /*oldLayout*/, ImageLayout newLayout)
	{
		if (!IsValid())
			return;

		const auto& dx12Texture = Cast<const Dx12RHITexture&>(texture);

		const D3D12_RESOURCE_STATES afterState = ToD3D12ResourceState(newLayout);
		const D3D12_RESOURCE_STATES beforeState = dx12Texture.GetState();
		if (beforeState == afterState)
			return;

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = dx12Texture.GetResource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = beforeState;
		barrier.Transition.StateAfter = afterState;

		Get()->ResourceBarrier(1, &barrier);
		dx12Texture.SetState(afterState);
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

	void Dx12RHICommandBuffer::PipelineBarrier(std::span<const TextureBarrier> barriers)
	{
		if (!IsValid() || barriers.empty())
			return;

		// Unlike Vulkan, D3D12 resource states are per-resource (no shared src/dst stage mask
		// for the call), so batching is just building the array and skipping resources that
		// are already in the target state -- same no-op guard TransitionImageLayout has.
		std::vector<D3D12_RESOURCE_BARRIER> dxBarriers;
		dxBarriers.reserve(barriers.size());

		for (const TextureBarrier& barrier : barriers)
		{
			const auto& dx12Texture = Cast<const Dx12RHITexture&>(*barrier.texture);

			const D3D12_RESOURCE_STATES afterState = ToD3D12ResourceState(barrier.newLayout);
			const D3D12_RESOURCE_STATES beforeState = dx12Texture.GetState();
			if (beforeState == afterState)
				continue;

			D3D12_RESOURCE_BARRIER dxBarrier{};
			dxBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			dxBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			dxBarrier.Transition.pResource = dx12Texture.GetResource();
			dxBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			dxBarrier.Transition.StateBefore = beforeState;
			dxBarrier.Transition.StateAfter = afterState;
			dxBarriers.push_back(dxBarrier);

			dx12Texture.SetState(afterState);
		}

		if (!dxBarriers.empty())
			Get()->ResourceBarrier(static_cast<UINT>(dxBarriers.size()), dxBarriers.data());
	}

	namespace
	{
		// Unlike Vulkan buffer barriers (memory-access-only, no resource "layout"), D3D12
		// buffers have real resource states that must be transitioned explicitly
		D3D12_RESOURCE_STATES BufferAccessToD3D12State(MemoryAccessFlags access, PipelineStageFlags stage)
		{
			D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

			if (access.Contains(MemoryAccess::TransferWrite))
				state |= D3D12_RESOURCE_STATE_COPY_DEST;
			if (access.Contains(MemoryAccess::TransferRead))
				state |= D3D12_RESOURCE_STATE_COPY_SOURCE;
			if (access.Contains(MemoryAccess::ShaderWrite))
				state |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			if (access.Contains(MemoryAccess::UniformRead) || access.Contains(MemoryAccess::VertexAttributeRead))
				state |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			if (access.Contains(MemoryAccess::IndexRead))
				state |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
			if (access.Contains(MemoryAccess::ShaderRead))
			{
				if (stage.Contains(PipelineStage::FragmentShader))
					state |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				if (stage.Contains(PipelineStage::VertexShader) ||
					stage.Contains(PipelineStage::ComputeShader) ||
					stage.Contains(PipelineStage::GeometryShader) ||
					stage.Contains(PipelineStage::TessellationControlShader) ||
					stage.Contains(PipelineStage::TessellationEvaluationShader))
					state |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			}

			return state;
		}
	} // namespace

	void Dx12RHICommandBuffer::PipelineBarrier(const Buffer& buffer,
											   PipelineStageFlags /*srcStage*/,
											   PipelineStageFlags dstStage,
											   MemoryAccessFlags /*srcAccess*/,
											   MemoryAccessFlags dstAccess)
	{
		if (!IsValid())
			return;

		const auto& dx12Buffer = Cast<const Dx12RHIBuffer&>(buffer);
		const D3D12_RESOURCE_STATES beforeState = dx12Buffer.GetState();
		const D3D12_RESOURCE_STATES afterState = BufferAccessToD3D12State(dstAccess, dstStage);

		D3D12_RESOURCE_BARRIER barrier{};
		if (beforeState == afterState)
		{
			if (beforeState != D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
				return;

			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrier.UAV.pResource = dx12Buffer.GetResource();
		}
		else
		{
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = dx12Buffer.GetResource();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = beforeState;
			barrier.Transition.StateAfter = afterState;
		}

		Get()->ResourceBarrier(1, &barrier);
		dx12Buffer.SetState(afterState);
	}

	void Dx12RHICommandBuffer::BindComputePipeline(const Pipeline& pipeline)
	{
		if (!IsValid() || !m_device)
			return;

		SetHeapsAndRootSignature(Cast<const Dx12RHIPipeline&>(pipeline), /* isCompute */ true);
	}

	void Dx12RHICommandBuffer::BindComputeDescriptorSet(const PipelineLayout& layout, const DescriptorSet& set)
	{
		if (!IsValid())
			return;

		const auto& dx12Layout = Cast<const Dx12RHIPipelineLayout&>(layout);
		const auto& dx12Set = Cast<const Dx12RHIDescriptorSet&>(set);
		BindDescriptorSetImpl(dx12Layout.GetRootSignature(), dx12Set, 0, /* isCompute */ true);
	}

	void Dx12RHICommandBuffer::Dispatch(UInt32 groupCountX, UInt32 groupCountY, UInt32 groupCountZ)
	{
		if (!IsValid())
			return;

		Get()->Dispatch(groupCountX, groupCountY, groupCountZ);
	}

	namespace
	{
#ifndef D3D12_EVENT_METADATA
		constexpr UINT kD3d12EventMetadata = 0x2;
#else
		constexpr UINT kD3d12EventMetadata = D3D12_EVENT_METADATA;
#endif
		constexpr UINT64 kPixTypeBitShift = 10;
		constexpr UINT64 kPixBeginEventNoArgs = 0x002;
		constexpr std::size_t kPixRecordQwords = 64;

		UINT64 EncodePixEventInfo(UINT64 eventType)
		{
			return (eventType & 0x3FFull) << kPixTypeBitShift;
		}

		UINT64 EncodePixAnsiStringInfo()
		{
			// alignment=0, copyChunkSize=8, isANSI=TRUE, isShortcut=FALSE
			return (8ull << 55) | (1ull << 54);
		}

		void CopyPixAnsiString(UINT64*& dest, const UINT64* limit, const char* text)
		{
			*dest++ = EncodePixAnsiStringInfo();
			while (dest < limit)
			{
				UINT64 packed = 0;
				for (int i = 0; i < 8; ++i)
				{
					const auto c = static_cast<unsigned char>(*text++);
					if (c == 0)
					{
						*dest++ = packed;
						return;
					}
					packed |= static_cast<UINT64>(c) << (i * 8);
				}
				*dest++ = packed;
			}
		}

		UINT64 PackPixColor(float r, float g, float b)
		{
			const auto toU8 = [](float v) -> UINT64
			{
				return static_cast<UINT64>(std::clamp(v, 0.f, 1.f) * 255.f + 0.5f);
			};
			return 0xFF000000ull | (toU8(r) << 16) | (toU8(g) << 8) | toU8(b);
		}
	} // namespace

	void Dx12RHICommandBuffer::BeginDebugLabel(const char* name, float r, float g, float b)
	{
		if (!IsValid())
			return;

		UINT64 buffer[kPixRecordQwords]{};
		UINT64* dest = buffer;
		const UINT64* limit = buffer + kPixRecordQwords - 2;
		*dest++ = EncodePixEventInfo(kPixBeginEventNoArgs);
		*dest++ = PackPixColor(r, g, b);
		if (name != nullptr && name[0] != '\0')
			CopyPixAnsiString(dest, limit, name);
		else
			*dest++ = 0;
		*dest = 0;

		const UINT size = static_cast<UINT>(reinterpret_cast<std::uint8_t*>(dest) - reinterpret_cast<std::uint8_t*>(buffer));
		Get()->BeginEvent(kD3d12EventMetadata, buffer, size);
	}

	void Dx12RHICommandBuffer::EndDebugLabel()
	{
		if (!IsValid())
			return;
		Get()->EndEvent();
	}

	void* Dx12RHICommandBuffer::GetNativeHandle() const
	{
		return static_cast<void*>(Get());
	}
} // namespace cct::gfx::rhi
