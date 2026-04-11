//
// Created by arthur on 01/09/2025.
//

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIBuffer/Dx12RHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHITexture/Dx12RHITexture.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIBuffer::Dx12RHIBuffer(Dx12RHIDevice& device, rhi::BufferUsageFlags usage, UInt32 size, bool allowMapping) :
		m_device(&device),
		m_size(size),
		m_allowMapping(allowMapping)
	{
		// CBV requires 256-byte aligned size
		bool isUniform = (usage & static_cast<BufferUsageFlags>(BufferUsage::Uniform)) != 0;
		UInt32 allocSize = isUniform ? (size + 255) & ~255u : size;
		m_size = allocSize;

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = allowMapping ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
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
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_RESOURCE_STATES initialState = allowMapping ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COMMON;

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
	}

	bool Dx12RHIBuffer::CopyTo(const Texture& texture)
	{
		const auto& dx12Texture = Cast<const Dx12RHITexture&>(texture);
		auto* d3dDevice = m_device->dx12::Device::Get();

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
		HRESULT hr = d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
		if (FAILED(hr))
			return false;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
		hr = d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
		if (FAILED(hr))
			return false;

		// Get texture layout info for the copy
		D3D12_RESOURCE_DESC texDesc = dx12Texture.GetResource()->GetDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
		UINT numRows = 0;
		UINT64 rowSizeInBytes = 0;
		UINT64 totalBytes = 0;
		d3dDevice->GetCopyableFootprints(&texDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

		// Copy buffer to texture
		D3D12_TEXTURE_COPY_LOCATION dst{};
		dst.pResource = dx12Texture.GetResource();
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION src{};
		src.pResource = m_resource.Get();
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint = footprint;

		// Transition texture from COMMON to COPY_DEST
		D3D12_RESOURCE_BARRIER barrierToCopyDst{};
		barrierToCopyDst.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrierToCopyDst.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrierToCopyDst.Transition.pResource = dx12Texture.GetResource();
		barrierToCopyDst.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrierToCopyDst.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		barrierToCopyDst.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		cmdList->ResourceBarrier(1, &barrierToCopyDst);

		cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

		// Transition texture from COPY_DEST to PIXEL_SHADER_RESOURCE
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = dx12Texture.GetResource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		cmdList->ResourceBarrier(1, &barrier);

		hr = cmdList->Close();
		if (FAILED(hr))
			return false;

		// Execute on the device's persistent upload queue and wait for completion
		m_device->ExecuteAndWait(cmdList.Get());
		return true;
	}

	bool Dx12RHIBuffer::Map(Byte** data)
	{
		if (!m_allowMapping || !m_resource)
			return false;

		D3D12_RANGE readRange = { 0, 0 }; // We don't need to read
		HRESULT hr = m_resource->Map(0, &readRange, reinterpret_cast<void**>(data));
		return SUCCEEDED(hr);
	}

	void Dx12RHIBuffer::UnMap()
	{
		if (!m_allowMapping || !m_resource)
			return;

		m_resource->Unmap(0, nullptr);
	}
}
