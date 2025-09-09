//
// Created by arthur on 09/04/2026.
//

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIQueue/Dx12RHIQueue.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFence/Dx12RHIFence.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIQueue::Dx12RHIQueue(ID3D12CommandQueue* queue, Dx12RHIDevice& device) :
		m_queue(queue),
		m_device(&device)
	{
	}

	void Dx12RHIQueue::Submit(CommandBuffer& cmdBuf, Fence* fence)
	{
		const auto& dx12CmdBuf = Cast<const Dx12RHICommandBuffer&>(cmdBuf);

		ID3D12CommandList* cmdLists[] = { dx12CmdBuf.Get() };
		m_queue->ExecuteCommandLists(1, cmdLists);

		if (fence)
		{
			auto& dx12RHIFence = Cast<Dx12RHIFence&>(*fence);
			UInt64 signalValue = dx12RHIFence.GetFence().GetCompletedValue();
			m_queue->Signal(dx12RHIFence.GetFence().Get(), signalValue);
		}
	}
}
