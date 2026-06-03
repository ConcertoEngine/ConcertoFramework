//
// Created by arthur on 24/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDESCRIPTORSETLAYOUT_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDESCRIPTORSETLAYOUT_HPP

#include <vector>

#include "Concerto/Graphics/Backend/Dx12/Dx12Helpers.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/DescriptorSetLayout.hpp"

namespace cct::gfx::rhi
{
	/**
	 * @brief DX12 implementation of DescriptorSetLayout
	 * Stores binding information and DX12-specific descriptor ranges
	 */
	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIDescriptorSetLayout : public DescriptorSetLayout
	{
	public:
		explicit Dx12RHIDescriptorSetLayout(std::vector<cct::gfx::DescriptorSetLayoutBinding> bindings);

		const std::vector<cct::gfx::DescriptorSetLayoutBinding>& GetBindings() const override;

		// DX12-specific: Get CBV/SRV/UAV descriptor ranges
		const std::vector<D3D12_DESCRIPTOR_RANGE1>& GetDx12Ranges() const noexcept
		{
			return m_dx12Ranges;
		}

		// DX12-specific: Get sampler descriptor ranges (separate heap)
		const std::vector<D3D12_DESCRIPTOR_RANGE1>& GetDx12SamplerRanges() const noexcept
		{
			return m_dx12SamplerRanges;
		}

		// Get combined shader visibility from all bindings
		D3D12_SHADER_VISIBILITY GetShaderVisibility() const noexcept
		{
			return m_shaderVisibility;
		}

		// Get total number of descriptors across all bindings
		UINT GetTotalDescriptorCount() const noexcept
		{
			return m_totalDescriptorCount;
		}

		// Check if this layout contains samplers
		bool HasSamplers() const noexcept
		{
			return !m_dx12SamplerRanges.empty();
		}

	private:
		void BuildDx12Ranges();

		std::vector<cct::gfx::DescriptorSetLayoutBinding> m_bindings;
		std::vector<D3D12_DESCRIPTOR_RANGE1> m_dx12Ranges; // CBV/SRV/UAV ranges
		std::vector<D3D12_DESCRIPTOR_RANGE1> m_dx12SamplerRanges; // Sampler ranges
		D3D12_SHADER_VISIBILITY m_shaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		UINT m_totalDescriptorCount = 0;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDESCRIPTORSETLAYOUT_HPP
