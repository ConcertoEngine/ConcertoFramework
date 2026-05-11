//
// Created by arthur on 22/03/2026.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIPipelineLayout/Dx12RHIPipelineLayout.hpp"

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSetLayout/Dx12RHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIPipelineLayout::Dx12RHIPipelineLayout(Dx12RHIDevice& device, std::span<const std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts) :
		m_descriptorSetLayouts(descriptorSetLayouts.begin(), descriptorSetLayouts.end())
	{
		std::vector<dx12::Dx12RootSignature::DescriptorTableInfo> tables;
		tables.reserve(descriptorSetLayouts.size());

		for (UINT setIndex = 0; setIndex < static_cast<UINT>(descriptorSetLayouts.size()); ++setIndex)
		{
			const auto& layout = descriptorSetLayouts[setIndex];
			const auto& dx12Layout = static_cast<const Dx12RHIDescriptorSetLayout&>(*layout);

			// CBV/SRV/UAV descriptor table
			if (!dx12Layout.GetDx12Ranges().empty())
			{
				dx12::Dx12RootSignature::DescriptorTableInfo table;
				table.setIndex = setIndex;
				table.ranges = dx12Layout.GetDx12Ranges();
				// nzsl maps [set(N)] to RegisterSpace = N in DXIL
				for (auto& range : table.ranges)
					range.RegisterSpace = setIndex;
				table.visibility = dx12Layout.GetShaderVisibility();
				tables.push_back(std::move(table));
			}

			// Sampler descriptor table (separate heap type in DX12)
			if (!dx12Layout.GetDx12SamplerRanges().empty())
			{
				dx12::Dx12RootSignature::DescriptorTableInfo samplerTable;
				samplerTable.setIndex = setIndex;
				samplerTable.ranges = dx12Layout.GetDx12SamplerRanges();
				for (auto& range : samplerTable.ranges)
					range.RegisterSpace = setIndex;
				samplerTable.visibility = dx12Layout.GetShaderVisibility();
				samplerTable.isSamplerTable = true;
				tables.push_back(std::move(samplerTable));
			}
		}

		m_rootSignature.Build(device.Get(), tables);
	}

	const std::vector<std::shared_ptr<DescriptorSetLayout>>& Dx12RHIPipelineLayout::GetDescriptorSetLayouts() const
	{
		return m_descriptorSetLayouts;
	}
} // namespace cct::gfx::rhi
