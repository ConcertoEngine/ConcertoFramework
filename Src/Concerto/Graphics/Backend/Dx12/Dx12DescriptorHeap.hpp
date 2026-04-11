//
// Created by arthur on 25/10/2025.
//

#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <mutex>

#include "Concerto/Graphics/Backend/Dx12/Defines.hpp"
#include "Concerto/Graphics/Backend/Dx12/Dx12Helpers.hpp"

namespace cct::gfx::dx12
{
	using Microsoft::WRL::ComPtr;

	// Represents a handle to a descriptor in a heap
	struct DescriptorHandle
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ 0 };
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{ 0 };
		UINT index = 0;

		bool IsValid() const noexcept { return cpuHandle.ptr != 0; }
	};

	// A contiguous range of descriptors
	struct DescriptorRange
	{
		DescriptorHandle baseHandle;
		UINT count = 0;

		DescriptorHandle operator[](UINT offset) const noexcept
		{
			DescriptorHandle result = baseHandle;
			result.cpuHandle.ptr += offset * baseHandle.index; // Use index as increment size
			if (result.gpuHandle.ptr != 0)
				result.gpuHandle.ptr += offset * baseHandle.index;
			result.index = baseHandle.index + offset;
			return result;
		}
	};

	/**
	 * @brief DX12 Descriptor Heap wrapper
	 * Manages a descriptor heap with optional GPU visibility
	 */
	class CONCERTO_GRAPHICS_DX12_BACKEND_API Dx12DescriptorHeap
	{
	public:
		Dx12DescriptorHeap() = default;

		// Create a descriptor heap
		// @param device: D3D12 device
		// @param type: Type of descriptors (CBV_SRV_UAV or SAMPLER)
		// @param numDescriptors: Total capacity
		// @param shaderVisible: Whether this heap is GPU-visible
		void Initialize(ID3D12Device* device,
		                D3D12_DESCRIPTOR_HEAP_TYPE type,
		                UINT numDescriptors,
		                bool shaderVisible);

		// Get the D3D12 heap
		ID3D12DescriptorHeap* GetHeap() const noexcept { return m_heap.Get(); }

		// Get descriptor increment size
		UINT GetDescriptorSize() const noexcept { return m_descriptorSize; }

		// Get total capacity
		UINT GetCapacity() const noexcept { return m_capacity; }

		// Check if GPU-visible
		bool IsShaderVisible() const noexcept { return m_shaderVisible; }

		// Get heap type
		D3D12_DESCRIPTOR_HEAP_TYPE GetType() const noexcept { return m_type; }

		// Get a handle at a specific index
		DescriptorHandle GetHandle(UINT index) const noexcept;

		// Get the base CPU handle
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStart() const noexcept;

		// Get the base GPU handle (if shader-visible)
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStart() const noexcept;

	private:
		ComPtr<ID3D12DescriptorHeap> m_heap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		UINT m_capacity = 0;
		UINT m_descriptorSize = 0;
		bool m_shaderVisible = false;
	};

	/**
	 * @brief Frame-based circular allocator for GPU-visible descriptor heap
	 * Thread-safe allocation with frame-based recycling
	 */
	class CONCERTO_GRAPHICS_DX12_BACKEND_API Dx12DescriptorAllocator
	{
	public:
		Dx12DescriptorAllocator() = default;

		// Initialize the allocator
		// @param heap: The heap to allocate from
		// @param framesInFlight: Number of frames in flight for recycling
		void Initialize(Dx12DescriptorHeap* heap, UINT framesInFlight);

		// Allocate a range of descriptors (thread-safe)
		// @param count: Number of descriptors to allocate
		// @return Range of allocated descriptors, invalid if out of space
		DescriptorRange Allocate(UINT count);

		// Mark frame completion and recycle descriptors from old frames
		// @param frameIndex: Current frame index
		void BeginFrame(UINT frameIndex);

		// Reset allocator (e.g., after resize)
		void Reset();

		// Get remaining capacity for current frame
		UINT GetRemainingCapacity() const noexcept;

	private:
		struct FrameAllocation
		{
			UINT startOffset = 0;
			UINT count = 0;
		};

		Dx12DescriptorHeap* m_heap = nullptr;
		UINT m_framesInFlight = 0;
		UINT m_currentOffset = 0;
		std::vector<std::vector<FrameAllocation>> m_frameAllocations; // Per-frame allocation tracking
		mutable std::mutex m_allocationMutex;
	};
}
