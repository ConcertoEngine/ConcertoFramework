//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_FACTORY_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_FACTORY_HPP

#include <span>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/PhysicalDevice/PhysicalDevice.hpp"
#include "Concerto/Graphics/Core/PixelFormat.hpp"

namespace cct::gfx::dx12
{
	class CONCERTO_GRAPHICS_DX12_BACKEND_API Factory : public Object<IDXGIFactory4>
	{
	public:
		Factory();
		explicit Factory(bool enableDebugLayer);
		~Factory() override = default;

		Factory(const Factory&) = delete;
		Factory(Factory&&) = default;

		Factory& operator=(const Factory&) = delete;
		Factory& operator=(Factory&&) = default;

		HRESULT Create(bool enableDebugLayer);

		std::span<PhysicalDevice> EnumeratePhysicalDevices();

		static DXGI_FORMAT PixelFormatToDXGI(PixelFormat pixelFormat);
		static void SetLogger(Logger& logger);
	private:
		std::vector<PhysicalDevice> m_physicalDevices;
	};
} // cct::gfx::dx12

#endif //CONCERTO_GRAPHICS_BACKEND_D3D12_FACTORY_HPP
