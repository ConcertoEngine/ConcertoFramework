//
// Created by arthur on 24/10/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSetLayout/Dx12RHIDescriptorSetLayout.hpp"

#include "Concerto/Graphics/Backend/Dx12/Dx12Helpers.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIDescriptorSetLayout::Dx12RHIDescriptorSetLayout(std::vector<cct::gfx::DescriptorSetLayoutBinding> bindings) :
		m_bindings(std::move(bindings))
	{
		BuildDx12Ranges();
	}

	const std::vector<cct::gfx::DescriptorSetLayoutBinding>& Dx12RHIDescriptorSetLayout::GetBindings() const
	{
		return m_bindings;
	}

	void Dx12RHIDescriptorSetLayout::BuildDx12Ranges()
	{
		if (m_bindings.empty())
			return;

		m_totalDescriptorCount = 0;
		cct::gfx::ShaderStageFlags combinedStageFlags;

		for (const auto& binding : m_bindings)
		{
			combinedStageFlags |= binding.stageFlags;

			if (binding.descriptorType == cct::gfx::ShaderBindingType::Sampler ||
				binding.descriptorType == cct::gfx::ShaderBindingType::CombinedImageSampler)
			{
				// nzsl sampler2D generates both a Texture2D (SRV) and a SamplerState in HLSL.
				// DX12 requires these in separate descriptor tables (different heap types).

				// SRV range for the texture
				D3D12_DESCRIPTOR_RANGE1 srvRange{};
				srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				srvRange.NumDescriptors = binding.descriptorCount;
				srvRange.BaseShaderRegister = binding.binding;
				srvRange.RegisterSpace = 0;
				srvRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
				srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				m_dx12Ranges.push_back(srvRange);

				// Sampler range
				D3D12_DESCRIPTOR_RANGE1 samplerRange{};
				samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
				samplerRange.NumDescriptors = binding.descriptorCount;
				samplerRange.BaseShaderRegister = binding.binding;
				samplerRange.RegisterSpace = 0;
				samplerRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
				samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				m_dx12SamplerRanges.push_back(samplerRange);

				m_totalDescriptorCount += binding.descriptorCount * 2;
			}
			else
			{
				D3D12_DESCRIPTOR_RANGE_TYPE dx12RangeType = dx12::ShaderBindingTypeToDx12RangeType(binding.descriptorType);

				D3D12_DESCRIPTOR_RANGE1 range{};
				range.RangeType = dx12RangeType;
				range.NumDescriptors = binding.descriptorCount;
				range.BaseShaderRegister = binding.binding;
				range.RegisterSpace = 0;
				range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				m_dx12Ranges.push_back(range);
				m_totalDescriptorCount += binding.descriptorCount;
			}
		}

		m_shaderVisibility = dx12::ShaderStageToVisibility(combinedStageFlags);
	}
} // namespace cct::gfx::rhi
