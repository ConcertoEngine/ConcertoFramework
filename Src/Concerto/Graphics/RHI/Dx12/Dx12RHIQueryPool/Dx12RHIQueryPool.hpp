//
// Created by arthur on 10/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12_RHI_QUERYPOOL_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12_RHI_QUERYPOOL_HPP

#include <d3d12.h>
#include <wrl/client.h>

#include "Concerto/Graphics/RHI/QueryPool.hpp"

namespace cct::gfx::rhi
{
	class Dx12RHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIQueryPool final : public QueryPool
	{
	public:
		explicit Dx12RHIQueryPool(Dx12RHIDevice& device);
		~Dx12RHIQueryPool() override;

		void BeginFrame(CommandBuffer& cmd) override;
		void EndFrame(CommandBuffer& cmd) override;
		float ReadLastFrameMs() const override { return m_lastMs; }

	private:
		Dx12RHIDevice&                          m_device;
		Microsoft::WRL::ComPtr<ID3D12QueryHeap> m_queryHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource>  m_readbackBuffer;
		uint64_t*                               m_mappedData    = nullptr;
		uint64_t                                m_timestampFreq = 1;
		int                                     m_frameIndex    = 0;
		float                                   m_lastMs        = 0.0F;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DX12_DX12_RHI_QUERYPOOL_HPP
