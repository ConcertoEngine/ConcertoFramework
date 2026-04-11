//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_DESCRIPTOR_HEAP_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_DESCRIPTOR_HEAP_HPP

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"

namespace cct::gfx::dx12
{
	class CONCERTO_GRAPHICS_DX12_BACKEND_API DescriptorHeap : public Object<ID3D12DescriptorHeap>
	{
	public:
		DescriptorHeap() = default;
		DescriptorHeap(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
		~DescriptorHeap() override = default;

		DescriptorHeap(const DescriptorHeap&) = delete;
		DescriptorHeap(DescriptorHeap&&) = default;

		DescriptorHeap& operator=(const DescriptorHeap&) = delete;
		DescriptorHeap& operator=(DescriptorHeap&&) = default;

		HRESULT Create(Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	private:
	};
} // cct::gfx::dx12

#endif //CONCERTO_GRAPHICS_BACKEND_D3D12_DESCRIPTOR_HEAP_HPP
