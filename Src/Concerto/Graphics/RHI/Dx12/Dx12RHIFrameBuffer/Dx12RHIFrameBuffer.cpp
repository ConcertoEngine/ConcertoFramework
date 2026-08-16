//
// Created by arthur on 22/03/2026.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFrameBuffer/Dx12RHIFrameBuffer.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIFrameBuffer::Dx12RHIFrameBuffer(UInt32 width, UInt32 height,
										   std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles,
										   std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargetResources,
										   std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandle,
										   Microsoft::WRL::ComPtr<ID3D12Resource> depthResource,
										   bool isSwapchainTarget,
										   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap) :
		m_width(width),
		m_height(height),
		m_rtvHandles(std::move(rtvHandles)),
		m_dsvHandle(dsvHandle),
		m_renderTargetResources(std::move(renderTargetResources)),
		m_depthResource(std::move(depthResource)),
		m_isSwapchainTarget(isSwapchainTarget),
		m_rtvHeap(std::move(rtvHeap))
	{
	}
} // namespace cct::gfx::rhi
