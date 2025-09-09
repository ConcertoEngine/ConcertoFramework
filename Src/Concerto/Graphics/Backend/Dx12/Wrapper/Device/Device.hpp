//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_DEVICE_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_DEVICE_HPP

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"

namespace cct::gfx::dx12
{
	class PhysicalDevice;

	class CONCERTO_GRAPHICS_DX12_BACKEND_API Device : public Object<ID3D12Device>
	{
	public:
		Device() = default;
		Device(ID3D12Device& device, PhysicalDevice& physicalDevice);
		~Device() override = default;

		Device(const Device&) = delete;
		Device(Device&&) = default;

		Device& operator=(const Device&) = delete;
		Device& operator=(Device&&) = default;

		HRESULT Create(ID3D12Device& device, PhysicalDevice& physicalDevice);
		PhysicalDevice& GetPhysicalDevice() const;
	private:
		PhysicalDevice* m_physicalDevice;
	};
} // cct::gfx::dx12

#endif //CONCERTO_GRAPHICS_BACKEND_D3D12_DEVICE_HPP
