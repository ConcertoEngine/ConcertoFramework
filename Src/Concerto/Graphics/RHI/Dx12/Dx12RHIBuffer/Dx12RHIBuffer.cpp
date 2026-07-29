//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIBuffer/Dx12RHIBuffer.hpp"

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIBuffer::Dx12RHIBuffer(Dx12RHIDevice& device, rhi::BufferUsageFlags usage, UInt32 size, bool allowMapping) :
		m_device(&device),
		m_size(size),
		m_allowMapping(allowMapping)
	{
		// CBV requires 256-byte aligned size
		bool isUniform = usage.Contains(BufferUsage::Uniform);
		UInt32 allocSize = isUniform ? (size + 255) & ~255u : size;
		m_size = allocSize;

		const bool isStorage = usage.Contains(BufferUsage::Storage);
		const bool needsHostReadback = usage.Contains(BufferUsage::HostReadback);
		const bool needsReadbackShadow = allowMapping && needsHostReadback;
		const bool wantsUav = isStorage && (!allowMapping || needsHostReadback);

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = (allowMapping && !needsReadbackShadow) ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = allocSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = wantsUav
								 ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
								 : D3D12_RESOURCE_FLAG_NONE;

		const D3D12_RESOURCE_STATES initialState = (heapProperties.Type == D3D12_HEAP_TYPE_UPLOAD)
													   ? D3D12_RESOURCE_STATE_GENERIC_READ
													   : D3D12_RESOURCE_STATE_COMMON;

		HRESULT hr = device.Get()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			initialState,
			nullptr,
			IID_PPV_ARGS(&m_resource));

		CCT_ASSERT(SUCCEEDED(hr), "ConcertoGraphics: Failed to create DX12 buffer resource HRESULT={}", hr);

		if (SUCCEEDED(hr))
			m_gpuAddress = m_resource->GetGPUVirtualAddress();

		if (needsReadbackShadow && SUCCEEDED(hr))
		{
			D3D12_HEAP_PROPERTIES readbackHeapProperties = {};
			readbackHeapProperties.Type = D3D12_HEAP_TYPE_READBACK;
			readbackHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			readbackHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

			D3D12_RESOURCE_DESC readbackDesc = resourceDesc;
			readbackDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			HRESULT readbackHr = device.Get()->CreateCommittedResource(
				&readbackHeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&readbackDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&m_readbackResource));

			CCT_ASSERT(SUCCEEDED(readbackHr), "ConcertoGraphics: Failed to create DX12 buffer readback shadow HRESULT={}", readbackHr);
		}
	}

	void Dx12RHIBuffer::RecordReadback(CommandBuffer& cmd)
	{
		if (!m_readbackResource)
			return;

		auto* cmdList = Cast<Dx12RHICommandBuffer&>(cmd).Get();

		D3D12_RESOURCE_BARRIER toCopySrc{};
		toCopySrc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		toCopySrc.Transition.pResource = m_resource.Get();
		toCopySrc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		toCopySrc.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		toCopySrc.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		cmdList->ResourceBarrier(1, &toCopySrc);

		cmdList->CopyBufferRegion(m_readbackResource.Get(), 0, m_resource.Get(), 0, m_size);

		D3D12_RESOURCE_BARRIER backToCommon = toCopySrc;
		backToCommon.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		backToCommon.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
		cmdList->ResourceBarrier(1, &backToCommon);
	}

	bool Dx12RHIBuffer::Map(Byte** data)
	{
		if (!m_allowMapping || !m_resource)
			return false;

		auto* target = m_readbackResource ? m_readbackResource.Get() : m_resource.Get();
		const D3D12_RANGE readRange = m_readbackResource ? D3D12_RANGE{0, m_size} : D3D12_RANGE{0, 0};
		return SUCCEEDED(target->Map(0, &readRange, reinterpret_cast<void**>(data)));
	}

	void Dx12RHIBuffer::UnMap()
	{
		if (!m_allowMapping || !m_resource)
			return;

		auto* target = m_readbackResource ? m_readbackResource.Get() : m_resource.Get();
		target->Unmap(0, nullptr);
	}
} // namespace cct::gfx::rhi
