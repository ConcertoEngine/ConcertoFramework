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
		Dx12RHITexture(Dx12RHIDevice& device, PixelFormat format, Int32 width, Int32 height, bool allowUnorderedAccess = false);
		Dx12RHITexture(Dx12RHIDevice& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, DXGI_FORMAT format,
					   UInt32 width, UInt32 height);
		~Dx12RHITexture() override = default;

		[[nodiscard]] std::unique_ptr<TextureView> CreateView() const override;

		[[nodiscard]] ID3D12Resource* GetResource() const
		{
			return m_resource.Get();
		}
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const
		{
			return m_srvHandle;
		}
		[[nodiscard]] DXGI_FORMAT GetFormat() const
		{
			return m_format;
		}
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const;

		[[nodiscard]] D3D12_RESOURCE_STATES GetState() const
		{
			return m_currentState;
		}
		void SetState(D3D12_RESOURCE_STATES state) const
		{
			m_currentState = state;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
		mutable Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_srvHandle = {};
		mutable D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle = {};
		Dx12RHIDevice* m_device;
		DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
		mutable D3D12_RESOURCE_STATES m_currentState = D3D12_RESOURCE_STATE_COMMON;
	};

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHITextureView : public rhi::TextureView
	{
	public:
		explicit Dx12RHITextureView(const Dx12RHITexture& texture);

		[[nodiscard]] ID3D12Resource* GetResource() const
		{
			return m_resource;
		}
		[[nodiscard]] DXGI_FORMAT GetFormat() const
		{
			return m_format;
		}

	private:
		ID3D12Resource* m_resource;
		DXGI_FORMAT m_format;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHITEXTURE_HPP
