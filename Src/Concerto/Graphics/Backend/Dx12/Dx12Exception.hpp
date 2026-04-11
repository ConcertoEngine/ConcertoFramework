//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_EXCEPTION_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_EXCEPTION_HPP

#include <stdexcept>

namespace cct::gfx::dx12
{
	class Dx12Exception : public std::runtime_error
	{
	public:
		Dx12Exception(HRESULT hr)
			: std::runtime_error("DirectX 12 error"),
			m_errorCode(hr)
		{
		}

		HRESULT GetErrorCode() const
		{
			return m_errorCode;
		}
	private:
		HRESULT m_errorCode;
	};
} // namespace cct::gfx::dx12

#endif // CONCERTO_GRAPHICS_BACKEND_D3D12_EXCEPTION_HPP