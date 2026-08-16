//
// Created by arthur on 22/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIFRAMEBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIFRAMEBUFFER_HPP

#include <optional>
#include <vector>

#include "Concerto/Graphics/Backend/Dx12/Defines.hpp"
#include "Concerto/Graphics/RHI/FrameBuffer.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIFrameBuffer : public rhi::FrameBuffer
	{
	public:
		Dx12RHIFrameBuffer(UInt32 width, UInt32 height,
						   std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles,
						   std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargetResources,
						   std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandle = std::nullopt,
						   Microsoft::WRL::ComPtr<ID3D12Resource> depthResource = nullptr,
						   bool isSwapchainTarget = false,
						   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr);

		UInt32 GetWidth() const override
		{
			return m_width;
		}
		UInt32 GetHeight() const override
		{
			return m_height;
		}

		[[nodiscard]] const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& GetRTVHandles() const
		{
			return m_rtvHandles;
		}
		[[nodiscard]] const std::optional<D3D12_CPU_DESCRIPTOR_HANDLE>& GetDSVHandle() const
		{
			return m_dsvHandle;
		}
		[[nodiscard]] const std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>& GetRenderTargetResources() const
		{
			return m_renderTargetResources;
		}
		[[nodiscard]] ID3D12Resource* GetDepthResource() const
		{
			return m_depthResource.Get();
		}

		[[nodiscard]] bool IsSwapchainTarget() const
		{
			return m_isSwapchainTarget;
		}

	private:
		UInt32 m_width;
		UInt32 m_height;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_rtvHandles;
		std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> m_dsvHandle;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_renderTargetResources;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthResource;
		bool m_isSwapchainTarget;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHIFRAMEBUFFER_HPP
