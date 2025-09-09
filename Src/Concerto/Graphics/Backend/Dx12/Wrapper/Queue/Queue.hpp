//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_QUEUE_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_QUEUE_HPP

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"

namespace cct::gfx::dx12
{
	class CONCERTO_GRAPHICS_DX12_BACKEND_API Queue : public Object<ID3D12CommandQueue>
	{
	public:
		Queue() = default;
		Queue(Device& device, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_FLAGS flags);
		~Queue() override = default;

		Queue(const Queue&) = delete;
		Queue(Queue&&) = default;

		Queue& operator=(const Queue&) = delete;
		Queue& operator=(Queue&&) = default;

		HRESULT Create(Device& device, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_FLAGS flags);
	private:
	};
} // cct::gfx::dx12

#endif //CONCERTO_GRAPHICS_BACKEND_D3D12_QUEUE_HPP
