//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_PHYSICAL_DEVICE_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_PHYSICAL_DEVICE_HPP

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"

namespace cct::gfx::dx12
{
	class Factory;

	class CONCERTO_GRAPHICS_DX12_BACKEND_API PhysicalDevice : public Object<IDXGIAdapter1>
	{
	public:
		PhysicalDevice();
		PhysicalDevice(IDXGIAdapter1& adapter, Factory& factory);
		~PhysicalDevice() override = default;

		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice(PhysicalDevice&&) = default;

		PhysicalDevice& operator=(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(PhysicalDevice&&) = default;

		HRESULT Create(IDXGIAdapter1& adapter, Factory& factory);
		Factory& GetFactory() const;
		DXGI_ADAPTER_DESC GetDescription() const;

		Device CreateDevice();

	private:
		Factory* m_factory;
	};
} // cct::gfx::dx12

#endif //CONCERTO_GRAPHICS_BACKEND_D3D12_PHYSICAL_DEVICE_HPP
