//
// Created by arthur on 22/03/2026.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIPipeline/Dx12RHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIPipelineLayout/Dx12RHIPipelineLayout.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIPipeline::Dx12RHIPipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState, std::shared_ptr<Dx12RHIPipelineLayout> pipelineLayout) :
		m_pipelineState(std::move(pipelineState)),
		m_pipelineLayout(std::move(pipelineLayout))
	{
		CCT_ASSERT(m_pipelineState, "ConcertoGraphics: Invalid DX12 pipeline state");
		CCT_ASSERT(m_pipelineLayout, "ConcertoGraphics: Invalid DX12 pipeline layout");
	}
}
