//
// Created by arthur on 22/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIPIPELINE_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIPIPELINE_HPP

#include <memory>

#include "Concerto/Graphics/Backend/Dx12/Defines.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Pipeline.hpp"

namespace cct::gfx::rhi
{
	class Dx12RHIPipelineLayout;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIPipeline : public rhi::Pipeline
	{
	public:
		Dx12RHIPipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState, std::shared_ptr<Dx12RHIPipelineLayout> pipelineLayout);

		[[nodiscard]] ID3D12PipelineState* GetPipelineState() const
		{
			return m_pipelineState.Get();
		}
		[[nodiscard]] const Dx12RHIPipelineLayout& GetLayout() const
		{
			return *m_pipelineLayout;
		}
		[[nodiscard]] std::shared_ptr<Dx12RHIPipelineLayout> GetLayoutPtr() const
		{
			return m_pipelineLayout;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		std::shared_ptr<Dx12RHIPipelineLayout> m_pipelineLayout;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHIPIPELINE_HPP
