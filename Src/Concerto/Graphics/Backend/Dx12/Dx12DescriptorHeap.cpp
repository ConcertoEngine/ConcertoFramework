//
// Created by arthur on 25/10/2025.
//

#include "Concerto/Graphics/Backend/Dx12/Dx12DescriptorHeap.hpp"
#include <algorithm>

namespace cct::gfx::dx12
{
	void Dx12DescriptorHeap::Initialize(ID3D12Device* device,
	                                     D3D12_DESCRIPTOR_HEAP_TYPE type,
	                                     UINT numDescriptors,
	                                     bool shaderVisible)
	{
		m_type = type;
		m_capacity = numDescriptors;
		m_shaderVisible = shaderVisible;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type = type;
		heapDesc.NumDescriptors = numDescriptors;
		heapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;

		ThrowIfFailed(
			device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap)),
			"Failed to create descriptor heap"
		);

		m_descriptorSize = device->GetDescriptorHandleIncrementSize(type);
	}

	DescriptorHandle Dx12DescriptorHeap::GetHandle(UINT index) const noexcept
	{
		if (index >= m_capacity)
			return DescriptorHandle{}; // Invalid

		DescriptorHandle handle;
		handle.cpuHandle = GetCPUStart();
		handle.cpuHandle.ptr += index * m_descriptorSize;

		if (m_shaderVisible)
		{
			handle.gpuHandle = GetGPUStart();
			handle.gpuHandle.ptr += index * m_descriptorSize;
		}

		handle.index = m_descriptorSize; // Store increment size
		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Dx12DescriptorHeap::GetCPUStart() const noexcept
	{
		if (!m_heap)
			return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
		return m_heap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE Dx12DescriptorHeap::GetGPUStart() const noexcept
	{
		if (!m_heap || !m_shaderVisible)
			return D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
		return m_heap->GetGPUDescriptorHandleForHeapStart();
	}

	// ========== Dx12DescriptorAllocator ==========

	void Dx12DescriptorAllocator::Initialize(Dx12DescriptorHeap* heap, UINT framesInFlight)
	{
		m_heap = heap;
		m_framesInFlight = framesInFlight;
		m_currentOffset = 0;
		m_frameAllocations.resize(framesInFlight);
	}

	DescriptorRange Dx12DescriptorAllocator::Allocate(UINT count)
	{
		if (!m_heap || count == 0)
			return DescriptorRange{};

		std::lock_guard<std::mutex> lock(m_allocationMutex);

		const UINT capacity = m_heap->GetCapacity();

		// Check if we have enough space
		if (m_currentOffset + count > capacity)
		{
			// Out of space - return invalid range
			// In production, might trigger heap resize or wait for frame completion
			return DescriptorRange{};
		}

		DescriptorRange range;
		range.baseHandle = m_heap->GetHandle(m_currentOffset);
		range.count = count;

		m_currentOffset += count;

		return range;
	}

	void Dx12DescriptorAllocator::BeginFrame(UINT frameIndex)
	{
		std::lock_guard<std::mutex> lock(m_allocationMutex);

		const UINT frameSlot = frameIndex % m_framesInFlight;

		// Clear allocations for this frame slot (they're now old enough to recycle)
		m_frameAllocations[frameSlot].clear();

		// Record allocation for this frame
		if (m_currentOffset > 0)
		{
			FrameAllocation alloc;
			alloc.startOffset = 0;
			alloc.count = m_currentOffset;
			m_frameAllocations[frameSlot].push_back(alloc);
		}

		// Simple circular reset: wrap around when we reach the end
		// A more sophisticated allocator would track free ranges
		if (frameIndex >= m_framesInFlight)
		{
			m_currentOffset = 0; // Reset for next cycle
		}
	}

	void Dx12DescriptorAllocator::Reset()
	{
		std::lock_guard<std::mutex> lock(m_allocationMutex);
		m_currentOffset = 0;
		for (auto& frameAllocs : m_frameAllocations)
			frameAllocs.clear();
	}

	UINT Dx12DescriptorAllocator::GetRemainingCapacity() const noexcept
	{
		std::lock_guard<std::mutex> lock(m_allocationMutex);
		if (!m_heap)
			return 0;
		return m_heap->GetCapacity() - m_currentOffset;
	}
}
