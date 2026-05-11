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
										   Microsoft::WRL::ComPtr<ID3D12Resource> depthResource) :
		m_width(width),
		m_height(height),
		m_rtvHandles(std::move(rtvHandles)),
		m_dsvHandle(dsvHandle),
		m_renderTargetResources(std::move(renderTargetResources)),
		m_depthResource(std::move(depthResource))
	{
	}
} // namespace cct::gfx::rhi
