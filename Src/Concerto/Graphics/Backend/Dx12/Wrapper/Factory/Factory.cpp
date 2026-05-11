//
// Created by arthur on 01/09/2025.
//
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Factory/Factory.hpp"

#include <Concerto/Core/DynLib/DynLib.hpp>

namespace cct::gfx::dx12
{
	Factory::Factory()
	{
	}

	Factory::Factory(bool enableDebugLayer)
	{
		if (FAILED(Create(enableDebugLayer)))
			throw Dx12Exception(m_lastResult);
	}

	HRESULT Factory::Create(bool enableDebugLayer)
	{
		UInt32 dxgiFactoryFlags = 0;
		if (enableDebugLayer && DynLib().Load("dxgidebug.dll")) // check if developer SDK is installed
		{
			Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
			m_lastResult = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
			if (SUCCEEDED(m_lastResult))
			{
				debugController->EnableDebugLayer();
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
			else
			{
				CCT_ASSERT_FALSE("Could not create debug interface");
				return m_lastResult;
			}
		}

		m_lastResult = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_handle));

		return m_lastResult;
	}

	std::span<PhysicalDevice> Factory::EnumeratePhysicalDevices()
	{
		UInt32 adapterIndex = 0;
		do
		{
			IDXGIAdapter1* adapter;
			m_lastResult = m_handle->EnumAdapters1(adapterIndex, &adapter);
			if (SUCCEEDED(m_lastResult))
				m_physicalDevices.emplace_back(*adapter, *this);
			++adapterIndex;

		} while (m_lastResult != DXGI_ERROR_NOT_FOUND);

		return m_physicalDevices;
	}

	DXGI_FORMAT Factory::PixelFormatToDXGI(PixelFormat pixelFormat)
	{
		switch (pixelFormat)
		{
				// --- 8-bit formats ---
			case PixelFormat::R8uNorm:
				return DXGI_FORMAT_R8_UNORM;
			case PixelFormat::R8ui:
				return DXGI_FORMAT_R8_UINT;
			case PixelFormat::R8sNorm:
				return DXGI_FORMAT_R8_SNORM;
			case PixelFormat::R8i:
				return DXGI_FORMAT_R8_SINT;
			case PixelFormat::R8_SRGB:
				return DXGI_FORMAT_R8_UNORM; // No DXGI SRGB for single-channel, use UNORM

			case PixelFormat::RG8uNorm:
				return DXGI_FORMAT_R8G8_UNORM;
			case PixelFormat::RG8ui:
				return DXGI_FORMAT_R8G8_UINT;
			case PixelFormat::R8G8sNorm:
				return DXGI_FORMAT_R8G8_SNORM;
			case PixelFormat::R8G8i:
				return DXGI_FORMAT_R8G8_SINT;
			case PixelFormat::R8G8_SRGB:
				return DXGI_FORMAT_R8G8_UNORM; // no DXGI SRGB version

			case PixelFormat::RGB8uNorm:
				throw std::runtime_error("DXGI does not support 24-bit RGB format directly");
			case PixelFormat::RGB8ui:
				throw std::runtime_error("DXGI does not support 24-bit RGB format directly");
			case PixelFormat::RGB8sNorm:
				throw std::runtime_error("DXGI does not support 24-bit RGB format directly");
			case PixelFormat::RGB8i:
				throw std::runtime_error("DXGI does not support 24-bit RGB format directly");
			case PixelFormat::RGB8_SRGB:
				throw std::runtime_error("DXGI does not support 24-bit RGB SRGB format directly");

			case PixelFormat::BGRuNorm:
				return DXGI_FORMAT_B8G8R8X8_UNORM; // best fit (with alpha ignored)
			case PixelFormat::BGRui:
				throw std::runtime_error("DXGI does not support packed BGR UINT directly");
			case PixelFormat::BGRsNorm:
				throw std::runtime_error("DXGI does not support packed BGR SNORM directly");
			case PixelFormat::BGRi:
				throw std::runtime_error("DXGI does not support packed BGR SINT directly");
			case PixelFormat::BGR_SRGB:
				return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

			case PixelFormat::RGBA8uNorm:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case PixelFormat::RGBA8ui:
				return DXGI_FORMAT_R8G8B8A8_UINT;
			case PixelFormat::RGBA8sNorm:
				return DXGI_FORMAT_R8G8B8A8_SNORM;
			case PixelFormat::RGBA8i:
				return DXGI_FORMAT_R8G8B8A8_SINT;
			case PixelFormat::RGBA8_SRGB:
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

			case PixelFormat::BGRA8uNorm:
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			case PixelFormat::BGRA8ui:
				throw std::runtime_error("DXGI does not support BGRA UINT format");
			case PixelFormat::BGRA8sNorm:
				throw std::runtime_error("DXGI does not support BGRA SNORM format");
			case PixelFormat::BGRA8i:
				throw std::runtime_error("DXGI does not support BGRA SINT format");
			case PixelFormat::BGRA8_SRGB:
				return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

			// --- 16-bit formats ---
			case PixelFormat::R16uNorm:
				return DXGI_FORMAT_R16_UNORM;
			case PixelFormat::R16ui:
				return DXGI_FORMAT_R16_UINT;
			case PixelFormat::R16sNorm:
				return DXGI_FORMAT_R16_SNORM;
			case PixelFormat::R16i:
				return DXGI_FORMAT_R16_SINT;
			case PixelFormat::R16f:
				return DXGI_FORMAT_R16_FLOAT;

			case PixelFormat::R16G16uNorm:
				return DXGI_FORMAT_R16G16_UNORM;
			case PixelFormat::R16G16ui:
				return DXGI_FORMAT_R16G16_UINT;
			case PixelFormat::R16G16sNorm:
				return DXGI_FORMAT_R16G16_SNORM;
			case PixelFormat::R16G16i:
				return DXGI_FORMAT_R16G16_SINT;
			case PixelFormat::R16G16f:
				return DXGI_FORMAT_R16G16_FLOAT;

			case PixelFormat::RGBA16uNorm:
				return DXGI_FORMAT_R16G16B16A16_UNORM;
			case PixelFormat::RGBA16ui:
				return DXGI_FORMAT_R16G16B16A16_UINT;
			case PixelFormat::RGBA16sNorm:
				return DXGI_FORMAT_R16G16B16A16_SNORM;
			case PixelFormat::RGBA16i:
				return DXGI_FORMAT_R16G16B16A16_SINT;
			case PixelFormat::RGBA16f:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;

			// --- 32-bit formats ---
			case PixelFormat::R32ui:
				return DXGI_FORMAT_R32_UINT;
			case PixelFormat::R32i:
				return DXGI_FORMAT_R32_SINT;
			case PixelFormat::R32f:
				return DXGI_FORMAT_R32_FLOAT;

			case PixelFormat::RG32u:
				return DXGI_FORMAT_R32G32_UINT;
			case PixelFormat::RG32i:
				return DXGI_FORMAT_R32G32_SINT;
			case PixelFormat::RG32f:
				return DXGI_FORMAT_R32G32_FLOAT;

			case PixelFormat::RGB32ui:
				return DXGI_FORMAT_R32G32B32_UINT;
			case PixelFormat::RGB32i:
				return DXGI_FORMAT_R32G32B32_SINT;
			case PixelFormat::RGB32f:
				return DXGI_FORMAT_R32G32B32_FLOAT;

			case PixelFormat::RGBA32ui:
				return DXGI_FORMAT_R32G32B32A32_UINT;
			case PixelFormat::RGBA32i:
				return DXGI_FORMAT_R32G32B32A32_SINT;
			case PixelFormat::RGBA32f:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case PixelFormat::R64ui:
			case PixelFormat::RG64ui:
			case PixelFormat::RGB64ui:
			case PixelFormat::RGBA64ui:
			case PixelFormat::R64i:
			case PixelFormat::RG64i:
			case PixelFormat::RGB64i:
			case PixelFormat::RGBA64i:
			case PixelFormat::R64f:
			case PixelFormat::RG64f:
			case PixelFormat::RGB64f:
			case PixelFormat::RGBA64f:
				throw std::runtime_error("DXGI does not support 64-bit per-channel formats");

			case PixelFormat::D32f:
				return DXGI_FORMAT_D32_FLOAT;

			default:
				throw std::runtime_error("Unsupported PixelFormat enum value: " + std::to_string(static_cast<int>(pixelFormat)));
		}
	}

	void Factory::SetLogger(Logger& logger)
	{
		Logger::SetContext(&logger);
	}
} // namespace cct::gfx::dx12
