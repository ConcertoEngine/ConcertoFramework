//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHITEXTURE_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHITEXTURE_HPP

#include "Concerto/Graphics/Backend/Dx12/Defines.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"

namespace cct::gfx::rhi
{
	class Dx12RHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHITexture : public rhi::Texture
	{
	public:
		Dx12RHITexture(Dx12RHIDevice& device, PixelFormat format, Int32 width, Int32 height);
		~Dx12RHITexture() override = default;

		[[nodiscard]] ID3D12Resource* GetResource() const
		{
			return m_resource.Get();
		}
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const
		{
			return m_srvHandle;
		}
		[[nodiscard]] UInt32 GetWidth() const
		{
			return m_width;
		}
		[[nodiscard]] UInt32 GetHeight() const
		{
			return m_height;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
		D3D12_CPU_DESCRIPTOR_HANDLE m_srvHandle = {};
		Dx12RHIDevice* m_device;
		UInt32 m_width;
		UInt32 m_height;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHITEXTURE_HPP
