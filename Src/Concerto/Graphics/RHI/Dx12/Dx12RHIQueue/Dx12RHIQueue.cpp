//
// Created by arthur on 09/04/2026.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIQueue/Dx12RHIQueue.hpp"

#include <Concerto/Core/Cast.hpp>
#include <Concerto/Core/Logger/LogMacros.hpp>

#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFence/Dx12RHIFence.hpp"

namespace cct::gfx::rhi
{
	namespace
	{
		bool LogIfDeviceRemoved(Dx12RHIDevice& device, const char* where)
		{
			ID3D12Device* d3d = static_cast<dx12::Device&>(device).Get();
			if (!d3d)
				return true;
			const HRESULT reason = d3d->GetDeviceRemovedReason();
			if (SUCCEEDED(reason))
				return false;
			CCT_GFX_LOG_ERROR("Dx12RHIQueue", "D3D12 device removed at {} (HRESULT={:#x})", where,
							  static_cast<unsigned>(reason));
			return true;
		}
	} // namespace

	Dx12RHIQueue::Dx12RHIQueue(ID3D12CommandQueue* queue, Dx12RHIDevice& device) :
		m_queue(queue),
		m_device(&device)
	{
	}

	void Dx12RHIQueue::Submit(CommandBuffer& cmdBuf, Fence* fence)
	{
		if (!m_queue)
			return;

		if (LogIfDeviceRemoved(*m_device, "Submit (before ExecuteCommandLists)"))
			return;

		const auto& dx12CmdBuf = Cast<const Dx12RHICommandBuffer&>(cmdBuf);

		ID3D12CommandList* cmdLists[] = {dx12CmdBuf.Get()};
		m_queue->ExecuteCommandLists(1, cmdLists);

		if (LogIfDeviceRemoved(*m_device, "Submit (after ExecuteCommandLists)"))
			return;

		if (fence)
		{
			auto& dx12RHIFence = Cast<Dx12RHIFence&>(*fence);
			dx12RHIFence.GetFence().Signal(*m_queue);
		}
	}
} // namespace cct::gfx::rhi
