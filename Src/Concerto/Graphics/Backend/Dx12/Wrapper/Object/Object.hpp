//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_OBJECT_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_OBJECT_HPP

#include <type_traits>

#include "Concerto/Graphics/Backend/Dx12/Defines.hpp"

namespace cct::gfx::dx12
{
	class Device;

	template<typename Dx12Type>
	class Object
	{
	public:
		Object();
		virtual ~Object();

		Object(const Object&) = delete;
		Object(Object&&) noexcept;

		Object& operator=(const Object&) = delete;
		Object& operator=(Object&&) noexcept;

		[[nodiscard]] Dx12Type* Get() const;
		[[nodiscard]] bool IsValid() const;
		[[nodiscard]] Device* GetDevice() const requires (!std::is_same_v<Dx12Type, IDXGIFactory> && !std::is_same_v<Dx12Type, ID3D12Device>);
		[[nodiscard]] HRESULT GetLastResult() const;

		Dx12Type* operator->();
		const Dx12Type* operator->() const;

	protected:
		Microsoft::WRL::ComPtr<Dx12Type> m_handle;
		Device* m_device;
		mutable HRESULT m_lastResult;
	};
} // cct::gfx::dx12

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.inl"

#endif //CONCERTO_GRAPHICS_BACKEND_D3D12_OBJECT_HPP
