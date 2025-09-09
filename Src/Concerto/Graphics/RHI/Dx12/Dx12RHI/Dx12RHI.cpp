//
// Created by arthur on 01/09/2025.
//


#include "Concerto/Graphics/RHI/Dx12/Dx12RHI/Dx12RHI.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Factory/Factory.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/PhysicalDevice/PhysicalDevice.hpp"

#define CCT_FACTORY static_cast<cct::gfx::dx12::Factory*>(m_factory)

namespace cct::gfx::rhi
{
	Dx12RHI::~Dx12RHI()
	{
		delete CCT_FACTORY;
	}

	bool Dx12RHI::Create(rhi::ValidationLevel validationLevel)
	{
		m_factory = new dx12::Factory;
		return CCT_FACTORY->Create(validationLevel != ValidationLevel::None) == S_OK;
	}

	std::span<const rhi::DeviceInfo> Dx12RHI::EnumerateDevices()
	{
		if (!CCT_FACTORY->IsValid())
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Invalid Vulkan instance, please call VkRHI::Create before");
			return m_deviceInfos;
		}

		for (auto& physicalDevice : CCT_FACTORY->EnumeratePhysicalDevices())
		{
			DXGI_ADAPTER_DESC adapterDesc;
			physicalDevice->GetDesc(&adapterDesc);
			m_deviceInfos.emplace_back(DeviceInfo{
				.name = { reinterpret_cast<char*>(adapterDesc.Description) }, // Fixme
				.vendor = adapterDesc.VendorId,
				.type = DeviceType::Dedicated,
			});
		}

		return m_deviceInfos;
	}

	std::unique_ptr<rhi::Device> Dx12RHI::CreateDevice(std::size_t index)
	{
		auto& physicalDevice = CCT_FACTORY->EnumeratePhysicalDevices()[index];

		ID3D12Device* device;
		HRESULT result = D3D12CreateDevice(physicalDevice.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
		if (FAILED(result))
		{
			CCT_ASSERT(SUCCEEDED(result), "ConcertoGraphics: D3D12CreateDevice failed HRESULT={}", result);
			if (FAILED(result))
				throw dx12::Dx12Exception(result);
		}

#ifdef CCT_ENABLE_OBJECT_DEBUG
		ID3D12InfoQueue* infoQueue = nullptr;
		if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			infoQueue->Release();
		}

#endif

		return std::make_unique<Dx12RHIDevice>(*device, physicalDevice);
	}

	void Dx12RHI::SetLogger(Logger& logger)
	{
		Logger::SetContext(&logger);
		dx12::Factory::SetLogger(logger);
	}
}
