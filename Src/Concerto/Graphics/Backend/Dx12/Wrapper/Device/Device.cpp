//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"

#include <Concerto/Core/DynLib/DynLib.hpp>

namespace cct::gfx::dx12
{
	Device::Device(ID3D12Device& device, PhysicalDevice& physicalDevice)
	{
		if (FAILED(Create(device, physicalDevice)))
			throw Dx12Exception(m_lastResult);
	}

	HRESULT Device::Create(ID3D12Device& device, PhysicalDevice& physicalDevice)
	{
		m_handle = &device;
		m_physicalDevice = &physicalDevice;

		return S_OK;
	}

	PhysicalDevice& Device::GetPhysicalDevice() const
	{
		CCT_ASSERT(m_physicalDevice, "Invalid object state, 'Create' must be called");
		return *m_physicalDevice;
	}
} // namespace cct::gfx::dx12
