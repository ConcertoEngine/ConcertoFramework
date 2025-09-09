//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/Backend/Dx12/Wrapper/DescriptorHeap/DescriptorHeap.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"

namespace cct::gfx::dx12
{
	DescriptorHeap::DescriptorHeap(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		if (FAILED(Create(device, type, numDescriptors, flags)))
			throw Dx12Exception(m_lastResult);
	}

	HRESULT DescriptorHeap::Create(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		m_device = &device;

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {
			.Type = type,
			.NumDescriptors = numDescriptors,
			.Flags = flags,
			.NodeMask = 0
		};

		m_lastResult = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_handle));
		CCT_ASSERT(SUCCEEDED(m_lastResult), "ConcertoGraphics: CreateDescriptorHeap failed HRESULT={}", m_lastResult);

		return m_lastResult;
	}
} // namespace cct::gfx::dx12