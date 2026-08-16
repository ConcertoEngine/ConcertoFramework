//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Fence/Fence.hpp"

#include <Concerto/Core/Error/Error.hpp>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"

namespace cct::gfx::dx12
{
	Fence::Fence(Device& device, D3D12_FENCE_FLAGS flags)
	{
		if (FAILED(Create(device, flags)))
			throw Dx12Exception(m_lastResult);
	}

	HRESULT Fence::Create(Device& device, D3D12_FENCE_FLAGS flags)
	{
		m_device = &device;

		m_lastResult = device->CreateFence(0, flags, IID_PPV_ARGS(&m_handle));
		CCT_ASSERT(SUCCEEDED(m_lastResult), "ConcertoGraphics: CreateFence failed HRESULT={}", m_lastResult);

		m_completionEvent = CreateEvent(nullptr, false, false, nullptr);
		if (m_completionEvent == nullptr)
		{
			CCT_ASSERT_FALSE("Could not create Event, reason: {}", cct::Error::GetLastSystemErrorString());
			return S_FALSE;
		}

		return m_lastResult;
	}

	void Fence::Signal(ID3D12CommandQueue& queue) const
	{
		CCT_ASSERT(m_handle, "Invalid object state, 'Create' must be called");

		m_lastResult = queue.Signal(m_handle.Get(), m_nextSignalValue);
		CCT_ASSERT(SUCCEEDED(m_lastResult), "ConcertoGraphics: ID3D12CommandQueue::Signal failed HRESULT={}", m_lastResult);
		m_lastSignaledValue = m_nextSignalValue;
		++m_nextSignalValue;
	}

	void Fence::Wait() const
	{
		CCT_ASSERT(m_handle, "Invalid object state, 'Create' must be called");

		if (m_lastSignaledValue == 0)
			return;
		if (m_handle->GetCompletedValue() >= m_lastSignaledValue)
			return;

		m_lastResult = m_handle->SetEventOnCompletion(m_lastSignaledValue, m_completionEvent);
		auto res = WaitForSingleObject(m_completionEvent, INFINITE);
		CCT_ASSERT(res == WAIT_OBJECT_0, "WaitForSingleObject failed, reason: {}", cct::Error::GetLastSystemErrorString());
	}

	UInt64 Fence::GetCompletedValue() const
	{
		return m_lastSignaledValue;
	}
} // namespace cct::gfx::dx12