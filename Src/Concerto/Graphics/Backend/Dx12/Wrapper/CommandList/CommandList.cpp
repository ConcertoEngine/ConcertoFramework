//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandList/CommandList.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandAllocator/CommandAllocator.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"

namespace cct::gfx::dx12
{
	CommandList::CommandList(CommandAllocator& owner, D3D12_COMMAND_LIST_TYPE type)
	{
		if (FAILED(Create(owner, type)))
			throw Dx12Exception(m_lastResult);
	}

	HRESULT CommandList::Create(CommandAllocator& owner, D3D12_COMMAND_LIST_TYPE type)
	{
		m_device = owner.GetDevice();
		m_owner = &owner;

		m_lastResult = m_device->operator->()->CreateCommandList(0, type, owner.Get(), nullptr, IID_PPV_ARGS(&m_handle));
		CCT_ASSERT(SUCCEEDED(m_lastResult), "CreateCommandList failed HRESULT={}", m_lastResult);
		if (SUCCEEDED(m_lastResult))
		{
			m_lastResult = m_handle->Close();
			CCT_ASSERT(SUCCEEDED(m_lastResult), "ID3D12GraphicsCommandList::Close failed HRESULT={}", m_lastResult);
		}

		return m_lastResult;
	}

	CommandAllocator& CommandList::GetCommandAllocator() const
	{
		CCT_ASSERT(m_handle, "Invalid object state, 'Create' must be called");
		return *m_owner;
	}

	HRESULT CommandList::Close() const
	{
		CCT_ASSERT(m_handle, "Invalid object state, 'Create' must be called");

		m_lastResult = m_handle->Close();
		CCT_ASSERT(SUCCEEDED(m_lastResult), "ID3D12GraphicsCommandList::Close failed HRESULT={}", m_lastResult);
		return m_lastResult;
	}

	HRESULT CommandList::Reset() const
	{
		m_lastResult = m_handle->Reset(m_owner->Get(), nullptr);
		CCT_ASSERT(SUCCEEDED(m_lastResult), "ID3D12GraphicsCommandList::Reset failed HRESULT={}", m_lastResult);
		return m_lastResult;
	}
} // namespace cct::gfx::dx12