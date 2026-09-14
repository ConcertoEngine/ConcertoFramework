//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/Backend/Dx12/Wrapper/PhysicalDevice/PhysicalDevice.hpp"

#include <Concerto/Core/DynLib/DynLib.hpp>
#include <Concerto/Core/Types/Types.hpp>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"

namespace cct::gfx::dx12
{
	PhysicalDevice::PhysicalDevice() :
		m_factory(nullptr)
	{
	}

	PhysicalDevice::PhysicalDevice(IDXGIAdapter1& adapter, Factory& factory)
	{
		if (FAILED(Create(adapter, factory)))
			throw Dx12Exception(m_lastResult);
	}

	HRESULT PhysicalDevice::Create(IDXGIAdapter1& adapter, Factory& factory)
	{
		m_handle = &adapter;
		m_factory = &factory;

		return S_OK;
	}

	Factory& PhysicalDevice::GetFactory() const
	{
		CCT_ASSERT(m_factory, "Invalid object state, 'Create' must be called");
		return *m_factory;
	}

	DXGI_ADAPTER_DESC PhysicalDevice::GetDescription() const
	{
		DXGI_ADAPTER_DESC adapterDesc = {};
		m_lastResult = m_handle->GetDesc(&adapterDesc);
		if (FAILED(m_lastResult))
			throw Dx12Exception(m_lastResult);
		return adapterDesc;
	}

	Device PhysicalDevice::CreateDevice()
	{
		ID3D12Device* device;
		m_lastResult = D3D12CreateDevice(m_handle.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
		CCT_ASSERT(SUCCEEDED(m_lastResult), "ConcertoGraphics: D3D12CreateDevice failed HRESULT={}", m_lastResult);

		if (FAILED(m_lastResult))
			throw Dx12Exception(m_lastResult);

#ifdef CCT_ENABLE_OBJECT_DEBUG
		ID3D12InfoQueue* infoQueue = nullptr;
		if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
		{
			const BOOL breakOnSeverity = cct::IsDebuggerAttached() ? TRUE : FALSE;
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, breakOnSeverity);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, breakOnSeverity);
			infoQueue->Release();
		}
#endif

		return {*device, *this};
	}
} // namespace cct::gfx::dx12
