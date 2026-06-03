//
// Created by arthur on 09/04/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12_RHI_QUEUE_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12_RHI_QUEUE_HPP

#include <d3d12.h>

#include "Concerto/Graphics/RHI/Queue.hpp"

namespace cct::gfx::rhi
{
	class Dx12RHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIQueue : public rhi::Queue
	{
	public:
		Dx12RHIQueue(ID3D12CommandQueue* queue, Dx12RHIDevice& device);
		~Dx12RHIQueue() override = default;

		void Submit(CommandBuffer& cmdBuf, Fence* fence = nullptr) override;

	private:
		ID3D12CommandQueue* m_queue;
		Dx12RHIDevice* m_device;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12_RHI_QUEUE_HPP
