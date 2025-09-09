//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIFRAME_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIFRAME_HPP

#include "Concerto/Graphics/RHI/Frame.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIFrame : public rhi::Frame
	{
	public:
		Dx12RHIFrame() = default;

		void Present() override;
		rhi::CommandBuffer& GetCommandBuffer() override;
		std::size_t GetCurrentFrameIndex() override;
		rhi::FrameBuffer& GetFrameBuffer() override;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DX12_DX12RHIFRAME_HPP