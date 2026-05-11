//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHITexture/Dx12RHITexture.hpp"

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Factory/Factory.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

namespace cct::gfx::rhi
{
	Dx12RHITexture::Dx12RHITexture(Dx12RHIDevice& device, PixelFormat format, Int32 width, Int32 height) :
		m_device(&device),
		m_width(static_cast<UInt32>(width)),
		m_height(static_cast<UInt32>(height))
	{
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = static_cast<UINT64>(width);
		resourceDesc.Height = static_cast<UINT>(height);
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = dx12::Factory::PixelFormatToDXGI(format);
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = device.Get()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&m_resource));

		CCT_ASSERT(SUCCEEDED(hr), "ConcertoGraphics: Failed to create DX12 texture resource HRESULT={}", hr);

		if (SUCCEEDED(hr))
		{
			// Create SRV descriptor
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = resourceDesc.Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			// Note: SRV handle will be assigned when bound to a descriptor set
		}
	}
} // namespace cct::gfx::rhi
