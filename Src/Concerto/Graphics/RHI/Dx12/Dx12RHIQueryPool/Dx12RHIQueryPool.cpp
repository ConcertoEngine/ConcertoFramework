//
// Created by arthur on 10/05/2026.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIQueryPool/Dx12RHIQueryPool.hpp"

#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandList/CommandList.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIQueryPool::Dx12RHIQueryPool(Dx12RHIDevice& device) :
		m_device(device)
	{
		ID3D12Device* d3dDevice = static_cast<dx12::Device&>(device).Get();
		if (!d3dDevice)
		{
			CCT_RHI_LOG_WARN("Dx12RHIQueryPool: null D3D12 device; GPU timing unavailable");
			return;
		}

		// Query heap — 4 slots: 2 frames × 2 timestamps
		D3D12_QUERY_HEAP_DESC heapDesc{};
		heapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		heapDesc.Count = 4;
		heapDesc.NodeMask = 0;
		HRESULT hr = d3dDevice->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(&m_queryHeap));
		if (FAILED(hr))
		{
			CCT_RHI_LOG_WARN("Dx12RHIQueryPool: CreateQueryHeap failed ({:#x}); GPU timing unavailable",
							 static_cast<unsigned>(hr));
			return;
		}

		// Readback buffer — persistently mapped
		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_READBACK;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Width = sizeof(uint64_t) * 4;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		hr = d3dDevice->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE,
			&resDesc, D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr, IID_PPV_ARGS(&m_readbackBuffer));
		if (FAILED(hr))
		{
			CCT_RHI_LOG_WARN("Dx12RHIQueryPool: CreateCommittedResource failed ({:#x}); GPU timing unavailable",
							 static_cast<unsigned>(hr));
			m_queryHeap.Reset();
			return;
		}

		D3D12_RANGE readRange{0, sizeof(uint64_t) * 4};
		hr = m_readbackBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData));
		if (FAILED(hr))
		{
			CCT_RHI_LOG_WARN("Dx12RHIQueryPool: Map failed ({:#x}); GPU timing unavailable",
							 static_cast<unsigned>(hr));
			m_queryHeap.Reset();
			m_readbackBuffer.Reset();
			return;
		}

		// Timestamp frequency (ticks per second)
		ID3D12CommandQueue* queue = device.GetRenderQueue();
		if (queue)
			queue->GetTimestampFrequency(&m_timestampFreq);
	}

	Dx12RHIQueryPool::~Dx12RHIQueryPool()
	{
		if (m_readbackBuffer && m_mappedData)
		{
			D3D12_RANGE writeRange{0, 0};
			m_readbackBuffer->Unmap(0, &writeRange);
		}
	}

	void Dx12RHIQueryPool::BeginFrame(CommandBuffer& cmd)
	{
		if (!m_queryHeap)
			return;

		auto& cmdList = static_cast<dx12::CommandList&>(static_cast<Dx12RHICommandBuffer&>(cmd));

		// Read previous frame from persistently-mapped readback buffer
		const int prev = 1 - m_frameIndex;
		const uint64_t t0 = m_mappedData[prev * 2];
		const uint64_t t1 = m_mappedData[prev * 2 + 1];
		if (t1 > t0 && m_timestampFreq > 0)
			m_lastMs = static_cast<float>((t1 - t0) * 1000.0 / static_cast<double>(m_timestampFreq));

		// Write begin timestamp
		const auto cur = static_cast<UINT>(m_frameIndex * 2);
		cmdList.Get()->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, cur);
	}

	void Dx12RHIQueryPool::EndFrame(CommandBuffer& cmd)
	{
		if (!m_queryHeap)
			return;

		auto& cmdList = static_cast<dx12::CommandList&>(static_cast<Dx12RHICommandBuffer&>(cmd));
		ID3D12GraphicsCommandList* d3dCmd = cmdList.Get();

		const auto cur = static_cast<UINT>(m_frameIndex * 2);

		// Write end timestamp
		d3dCmd->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, cur + 1);

		// Resolve both timestamps to the readback buffer
		d3dCmd->ResolveQueryData(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP,
								 cur, 2,
								 m_readbackBuffer.Get(),
								 static_cast<UINT64>(cur) * sizeof(uint64_t));

		m_frameIndex ^= 1;
	}
} // namespace cct::gfx::rhi
