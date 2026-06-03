//
// Created by arthur on 25/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDESCRIPTORPOOL_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDESCRIPTORPOOL_HPP

#include <memory>
#include <vector>

#include "Concerto/Graphics/Backend/Dx12/Dx12DescriptorHeap.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::dx12
{
	class Device;
}

namespace cct::gfx::rhi
{
	class DescriptorSetLayout;
	class Dx12RHIDescriptorSet;

	/**
	 * @brief DX12 Descriptor Pool
	 * Manages GPU-visible and CPU staging heaps for descriptor allocation
	 */
	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIDescriptorPool
	{
	public:
		Dx12RHIDescriptorPool() = default;

		// Initialize the pool
		// @param device: DX12 device
		// @param maxSets: Maximum number of descriptor sets (unused in DX12, kept for API compatibility)
		// @param gpuHeapSize: Size of GPU-visible heap for CBV/SRV/UAV
		// @param samplerHeapSize: Size of GPU-visible sampler heap
		// @param framesInFlight: Number of frames in flight
		void Initialize(dx12::Device& device,
						UINT maxSets,
						UINT gpuHeapSize = 10000,
						UINT samplerHeapSize = 2048,
						UINT framesInFlight = 3);

		// Allocate a descriptor set from this pool
		// @param layout: The layout describing what descriptors to allocate
		// @return Allocated descriptor set (may be null if out of space)
		std::unique_ptr<Dx12RHIDescriptorSet> AllocateDescriptorSet(const DescriptorSetLayout& layout);

		// Begin a new frame
		void BeginFrame(UINT frameIndex);

		// Get the GPU-visible heap for CBV/SRV/UAV
		dx12::Dx12DescriptorHeap* GetGpuHeap() noexcept
		{
			return &m_gpuHeap;
		}

		// Get the GPU-visible sampler heap
		dx12::Dx12DescriptorHeap* GetSamplerHeap() noexcept
		{
			return &m_samplerHeap;
		}

		// Get the CPU staging heap
		dx12::Dx12DescriptorHeap* GetCpuStagingHeap() noexcept
		{
			return &m_cpuStagingHeap;
		}

		// Get the allocator for GPU heap
		dx12::Dx12DescriptorAllocator* GetGpuAllocator() noexcept
		{
			return &m_gpuAllocator;
		}

		// Get the allocator for sampler heap
		dx12::Dx12DescriptorAllocator* GetSamplerAllocator() noexcept
		{
			return &m_samplerAllocator;
		}

	private:
		dx12::Dx12DescriptorHeap m_gpuHeap; // GPU-visible CBV/SRV/UAV heap
		dx12::Dx12DescriptorHeap m_samplerHeap; // GPU-visible sampler heap
		dx12::Dx12DescriptorHeap m_cpuStagingHeap; // CPU-only staging heap

		dx12::Dx12DescriptorAllocator m_gpuAllocator; // Allocator for GPU heap
		dx12::Dx12DescriptorAllocator m_samplerAllocator; // Allocator for sampler heap

		dx12::Device* m_device = nullptr;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDESCRIPTORPOOL_HPP
