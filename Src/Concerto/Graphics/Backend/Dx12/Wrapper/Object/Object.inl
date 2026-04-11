//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_OBJECT_INL
#define CONCERTO_GRAPHICS_BACKEND_D3D12_OBJECT_INL

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"

namespace cct::gfx::dx12
{
	template <typename Dx12Type>
	Object<Dx12Type>::Object() :
		m_device(nullptr),
		m_lastResult(0)
	{
	}

	template <typename Dx12Type>
	Object<Dx12Type>::~Object()
	{
		if (IsValid())
			m_handle.Reset();
	}

	template <typename Dx12Type>
	Object<Dx12Type>::Object(Object&& other) noexcept
	{
		m_handle = std::exchange(other.m_handle, nullptr);
		m_device = std::exchange(other.m_device, nullptr);
		m_lastResult = std::exchange(other.m_lastResult, {});
	}

	template <typename Dx12Type>
	Object<Dx12Type>& Object<Dx12Type>::operator=(Object&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_device, other.m_device);

		return *this;
	}

	template <typename Dx12Type>
	Dx12Type* Object<Dx12Type>::Get() const
	{
		CCT_ASSERT(IsValid(), "The Dx12 object handle is null");
		return m_handle.Get();
	}

	template <typename Dx12Type>
	bool Object<Dx12Type>::IsValid() const
	{
		return m_handle != nullptr;
	}

	template <typename Dx12Type>
	Device* Object<Dx12Type>::GetDevice() const requires (!std::is_same_v<Dx12Type, IDXGIFactory> && !std::
		is_same_v<Dx12Type, ID3D12Device>)
	{
		CCT_ASSERT(IsValid(), "The Dx12 object handle is null");
		return m_device;
	}

	template <typename Dx12Type>
	HRESULT Object<Dx12Type>::GetLastResult() const
	{
		return m_lastResult;
	}

	template <typename Dx12Type>
	Dx12Type* Object<Dx12Type>::operator->()
	{
		CCT_ASSERT(IsValid(), "The Dx12 object handle is null");
		return m_handle.Get();
	}

	template <typename Dx12Type>
	const Dx12Type* Object<Dx12Type>::operator->() const
	{
		CCT_ASSERT(IsValid(), "The Dx12 object handle is null");
		return m_handle.Get();
	}
} // cct::gfx::dx12

#endif // CONCERTO_GRAPHICS_BACKEND_D3D12_OBJECT_INL
