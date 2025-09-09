//
// Created by arthur on 22/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIPIPELINELAYOUT_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIPIPELINELAYOUT_HPP

#include <memory>
#include <span>
#include <vector>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/PipelineLayout.hpp"
#include "Concerto/Graphics/Backend/Dx12/Dx12RootSignature.hpp"

namespace cct::gfx::rhi
{
	class Dx12RHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIPipelineLayout : public rhi::PipelineLayout
	{
	public:
		Dx12RHIPipelineLayout(Dx12RHIDevice& device, std::span<const std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts);

		const std::vector<std::shared_ptr<DescriptorSetLayout>>& GetDescriptorSetLayouts() const override;

		[[nodiscard]] dx12::Dx12RootSignature& GetRootSignature() { return m_rootSignature; }
		[[nodiscard]] const dx12::Dx12RootSignature& GetRootSignature() const { return m_rootSignature; }

	private:
		dx12::Dx12RootSignature m_rootSignature;
		std::vector<std::shared_ptr<DescriptorSetLayout>> m_descriptorSetLayouts;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DX12_DX12RHIPIPELINELAYOUT_HPP
