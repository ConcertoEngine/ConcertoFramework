//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandAllocator/CommandAllocator.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"

namespace cct::gfx::dx12
{
	CommandAllocator::CommandAllocator(Device& device, D3D12_COMMAND_LIST_TYPE type)
	{
		if (FAILED(Create(device, type)))
			throw Dx12Exception(m_lastResult);
	}

	HRESULT CommandAllocator::Create(Device& device, D3D12_COMMAND_LIST_TYPE type)
	{
		m_device = &device;
		m_type = type;

		m_lastResult = device->CreateCommandAllocator(m_type, IID_PPV_ARGS(&m_handle));
		CCT_ASSERT(SUCCEEDED(m_lastResult), "CreateCommandAllocator failed HRESULT={}", m_lastResult);

		return m_lastResult;
	}

	HRESULT CommandAllocator::Reset() const
	{
		CCT_ASSERT(m_handle, "Invalid object state, 'Create' must be called");

		m_lastResult = m_handle->Reset();
		CCT_ASSERT(m_lastResult == E_FAIL, "There is an actively recording command list referencing the command allocator, HRESULT={}", m_lastResult);

		return m_lastResult;
	}

	CommandList CommandAllocator::CreateCommandList()
	{
		return CommandList(*this, m_type);
	}
} // namespace cct::gfx::dx12