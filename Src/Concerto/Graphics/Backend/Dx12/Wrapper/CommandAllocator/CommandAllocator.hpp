//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_COMMAND_ALLOCATOR_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_COMMAND_ALLOCATOR_HPP

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/CommandList/CommandList.hpp"

namespace cct::gfx::dx12
{
	class CONCERTO_GRAPHICS_DX12_BACKEND_API CommandAllocator : public Object<ID3D12CommandAllocator>
	{
	public:
		CommandAllocator() = default;
		CommandAllocator(Device& device, D3D12_COMMAND_LIST_TYPE type);
		~CommandAllocator() override = default;

		CommandAllocator(const CommandAllocator&) = delete;
		CommandAllocator(CommandAllocator&&) = default;

		CommandAllocator& operator=(const CommandAllocator&) = delete;
		CommandAllocator& operator=(CommandAllocator&&) = default;

		HRESULT Create(Device& device, D3D12_COMMAND_LIST_TYPE type);

		HRESULT Reset() const;
		CommandList CreateCommandList();

	private:
		D3D12_COMMAND_LIST_TYPE m_type;
	};
} // cct::gfx::dx12

#endif //CONCERTO_GRAPHICS_BACKEND_D3D12_COMMAND_ALLOCATOR_HPP
