//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFrame/Dx12RHIFrame.hpp"

namespace cct::gfx::rhi
{
	void Dx12RHIFrame::Present()
	{
	}

	rhi::CommandBuffer& Dx12RHIFrame::GetCommandBuffer()
	{
		return *reinterpret_cast<CommandBuffer*>(nullptr);
	}

	std::size_t Dx12RHIFrame::GetCurrentFrameIndex()
	{
		return 0;
	}

	rhi::FrameBuffer& Dx12RHIFrame::GetFrameBuffer()
	{
		return *reinterpret_cast<FrameBuffer*>(nullptr);
	}
}
