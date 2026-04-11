//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_COMMAND_LIST_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_COMMAND_LIST_HPP

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"

namespace cct::gfx::dx12
{
	class CommandAllocator;

	class CONCERTO_GRAPHICS_DX12_BACKEND_API CommandList : public Object<ID3D12GraphicsCommandList>
	{
	public:
		CommandList() = default;
		CommandList(CommandAllocator& owner, D3D12_COMMAND_LIST_TYPE type);
		~CommandList() override = default;

		CommandList(const CommandList&) = delete;
		CommandList(CommandList&&) = default;

		CommandList& operator=(const CommandList&) = delete;
		CommandList& operator=(CommandList&&) = default;

		HRESULT Create(CommandAllocator& owner, D3D12_COMMAND_LIST_TYPE type);

		CommandAllocator& GetCommandAllocator() const;
		HRESULT Close() const;
		HRESULT Reset() const;
	private:
		CommandAllocator* m_owner;
	};
} // cct::gfx::dx12

#endif //CONCERTO_GRAPHICS_BACKEND_D3D12_COMMAND_LIST_HPP
