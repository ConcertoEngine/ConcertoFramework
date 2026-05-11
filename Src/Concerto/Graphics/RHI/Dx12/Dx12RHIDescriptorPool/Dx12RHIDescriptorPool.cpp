//
// Created by arthur on 25/10/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorPool/Dx12RHIDescriptorPool.hpp"

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSet/Dx12RHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSetLayout/Dx12RHIDescriptorSetLayout.hpp"

namespace cct::gfx::rhi
{
	void Dx12RHIDescriptorPool::Initialize(dx12::Device& device,
										   UINT maxSets,
										   UINT gpuHeapSize,
										   UINT samplerHeapSize,
										   UINT framesInFlight)
	{
		m_device = &device;

		// Initialize GPU-visible heap for CBV/SRV/UAV
		m_gpuHeap.Initialize(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, gpuHeapSize, true);
		m_gpuAllocator.Initialize(&m_gpuHeap, framesInFlight);

		// Initialize GPU-visible sampler heap
		m_samplerHeap.Initialize(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, samplerHeapSize, true);
		m_samplerAllocator.Initialize(&m_samplerHeap, framesInFlight);

		// Initialize CPU staging heap (larger, non shader-visible)
		// Used to build descriptors before copying to GPU
		m_cpuStagingHeap.Initialize(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, gpuHeapSize * 2, false);
	}

	std::unique_ptr<Dx12RHIDescriptorSet> Dx12RHIDescriptorPool::AllocateDescriptorSet(const DescriptorSetLayout& layout)
	{
		const auto& dx12Layout = static_cast<const Dx12RHIDescriptorSetLayout&>(layout);

		// Allocate CBV/SRV/UAV descriptors from GPU heap
		dx12::DescriptorRange gpuRange;
		UINT cbvSrvUavCount = static_cast<UINT>(dx12Layout.GetDx12Ranges().size());
		if (cbvSrvUavCount > 0)
		{
			gpuRange = m_gpuAllocator.Allocate(cbvSrvUavCount);
			if (!gpuRange.baseHandle.IsValid())
				return nullptr;
		}

		// Allocate sampler descriptors from sampler heap
		dx12::DescriptorRange samplerRange;
		UINT samplerCount = static_cast<UINT>(dx12Layout.GetDx12SamplerRanges().size());
		if (samplerCount > 0)
		{
			samplerRange = m_samplerAllocator.Allocate(samplerCount);
			if (!samplerRange.baseHandle.IsValid())
				return nullptr;
		}

		if (cbvSrvUavCount == 0 && samplerCount == 0)
			return nullptr;

		return std::make_unique<Dx12RHIDescriptorSet>(
			std::make_shared<Dx12RHIDescriptorSetLayout>(dx12Layout.GetBindings()),
			gpuRange,
			samplerRange,
			m_device);
	}

	void Dx12RHIDescriptorPool::BeginFrame(UINT frameIndex)
	{
		m_gpuAllocator.BeginFrame(frameIndex);
		m_samplerAllocator.BeginFrame(frameIndex);
	}
} // namespace cct::gfx::rhi
