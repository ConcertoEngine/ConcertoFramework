//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_FENCE_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_FENCE_HPP

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"

namespace cct::gfx::dx12
{
	class CONCERTO_GRAPHICS_DX12_BACKEND_API Fence : public Object<ID3D12Fence>
	{
	public:
		Fence() = default;
		Fence(Device& device, D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE);
		~Fence() override = default;

		Fence(const Fence&) = delete;
		Fence(Fence&&) = default;

		Fence& operator=(const Fence&) = delete;
		Fence& operator=(Fence&&) = default;

		HRESULT Create(Device& device, D3D12_FENCE_FLAGS flags = D3D12_FENCE_FLAG_NONE);

		void Signal(ID3D12CommandQueue& queue) const;
		void Wait() const;
		UInt64 GetCompletedValue() const;

	private:
		HANDLE m_completionEvent = nullptr;
		mutable UInt64 m_nextSignalValue = 1;
		mutable UInt64 m_lastSignaledValue = 0;
	};
} // namespace cct::gfx::dx12

#endif // CONCERTO_GRAPHICS_BACKEND_D3D12_FENCE_HPP
