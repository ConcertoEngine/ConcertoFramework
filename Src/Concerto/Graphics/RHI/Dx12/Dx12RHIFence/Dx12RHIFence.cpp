//
// Created by arthur on 09/04/2026.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFence/Dx12RHIFence.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIFence::Dx12RHIFence(Dx12RHIDevice& device) :
		m_fence(device)
	{
	}

	void Dx12RHIFence::Wait()
	{
		m_fence.Wait();
	}

	void Dx12RHIFence::Reset()
	{
		// DX12 fence uses a monotonic counter, no explicit reset needed
	}
}
