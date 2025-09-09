//
// Created by arthur on 09/04/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12_RHI_FENCE_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12_RHI_FENCE_HPP

#include "Concerto/Graphics/RHI/Fence.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Fence/Fence.hpp"

namespace cct::gfx::rhi
{
	class Dx12RHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIFence : public rhi::Fence
	{
	public:
		explicit Dx12RHIFence(Dx12RHIDevice& device);
		~Dx12RHIFence() override = default;

		void Wait() override;
		void Reset() override;

		dx12::Fence& GetFence() { return m_fence; }

	private:
		dx12::Fence m_fence;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DX12_DX12_RHI_FENCE_HPP
