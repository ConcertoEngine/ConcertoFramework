//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHITexture/Dx12RHITexture.hpp"

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Factory/Factory.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

namespace cct::gfx::rhi
{
	Dx12RHITexture::Dx12RHITexture(Dx12RHIDevice& device, PixelFormat format, Int32 width, Int32 height, bool allowUnorderedAccess) :
		m_device(&device),
		m_format(dx12::Factory::PixelFormatToDXGI(format))
	{
		m_width = static_cast<UInt32>(width);
		m_height = static_cast<UInt32>(height);

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
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		if (allowUnorderedAccess)
			resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		const D3D12_RESOURCE_STATES initialState = allowUnorderedAccess
													   ? D3D12_RESOURCE_STATE_UNORDERED_ACCESS
													   : D3D12_RESOURCE_STATE_COMMON;
		m_currentState = initialState;

		D3D12_CLEAR_VALUE clearValue{};
		clearValue.Format = resourceDesc.Format;
		clearValue.Color[0] = 0.f;
		clearValue.Color[1] = 0.f;
		clearValue.Color[2] = 0.f;
		clearValue.Color[3] = 0.f;

		HRESULT hr = device.Get()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			initialState,
			&clearValue,
			IID_PPV_ARGS(&m_resource));

		CCT_ASSERT(SUCCEEDED(hr), "ConcertoGraphics: Failed to create DX12 texture resource HRESULT={}", hr);
	}

	Dx12RHITexture::Dx12RHITexture(Dx12RHIDevice& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource,
								   DXGI_FORMAT format, UInt32 width, UInt32 height) :
		m_resource(std::move(resource)),
		m_device(&device),
		m_format(format),
		m_currentState(D3D12_RESOURCE_STATE_COMMON)
	{
		m_width = width;
		m_height = height;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Dx12RHITexture::GetRTVHandle() const
	{
		if (m_rtvHeap)
			return m_rtvHandle;
		if (!m_resource || !m_device)
			return {};

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.NumDescriptors = 1;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		const HRESULT hr = m_device->Get()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap));
		if (FAILED(hr) || !m_rtvHeap)
			return {};

		m_rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = m_format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		m_device->Get()->CreateRenderTargetView(m_resource.Get(), &rtvDesc, m_rtvHandle);
		return m_rtvHandle;
	}

	std::unique_ptr<TextureView> Dx12RHITexture::CreateView() const
	{
		return std::make_unique<Dx12RHITextureView>(*this);
	}

	Dx12RHITextureView::Dx12RHITextureView(const Dx12RHITexture& texture) :
		m_resource(texture.GetResource()),
		m_format(texture.GetFormat())
	{
	}
} // namespace cct::gfx::rhi
