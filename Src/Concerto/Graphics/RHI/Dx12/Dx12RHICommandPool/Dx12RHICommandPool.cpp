//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandPool/Dx12RHICommandPool.hpp"

#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandBuffer/Dx12RHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

namespace cct::gfx::rhi
{

	Dx12RHICommandPool::Dx12RHICommandPool(Dx12RHIDevice& device, CommandBufferUsage usage, D3D12_COMMAND_LIST_TYPE type) :
		CommandPool(usage),
		dx12::CommandAllocator(device, type),
		m_device(&device)
	{
	}

	std::unique_ptr<CommandBuffer> Dx12RHICommandPool::AllocateCommandBuffer()
	{
		D3D12_COMMAND_LIST_TYPE type = (m_usage == CommandBufferUsage::Secondary)
										   ? D3D12_COMMAND_LIST_TYPE_BUNDLE
										   : D3D12_COMMAND_LIST_TYPE_DIRECT;
		return std::make_unique<Dx12RHICommandBuffer>(*this, type, *m_device);
	}
} // namespace cct::gfx::rhi
