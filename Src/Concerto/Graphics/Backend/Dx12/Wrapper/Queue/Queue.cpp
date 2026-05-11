//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Queue/Queue.hpp"

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"

namespace cct::gfx::dx12
{
	Queue::Queue(Device& device, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_FLAGS flags)
	{
		if (FAILED(Create(device, type, flags)))
			throw Dx12Exception(m_lastResult);
	}

	HRESULT Queue::Create(Device& device, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_FLAGS flags)
	{
		m_device = &device;

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = flags;
		queueDesc.Type = type;

		m_lastResult = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_handle));
		CCT_ASSERT(SUCCEEDED(m_lastResult), "ConcertoGraphics: CreateCommandQueue failed HRESULT={}", m_lastResult);
		return m_lastResult;
	}
} // namespace cct::gfx::dx12