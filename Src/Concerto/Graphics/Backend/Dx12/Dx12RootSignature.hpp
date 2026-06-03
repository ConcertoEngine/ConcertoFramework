//
// Created by arthur on 25/10/2025.
//

#pragma once

#include <d3d12.h>
#include <memory>
#include <vector>

#include "Concerto/Graphics/Backend/Dx12/Defines.hpp"
#include "Concerto/Graphics/Backend/Dx12/Dx12Helpers.hpp"
#include <wrl/client.h>

namespace cct::gfx::dx12
{
	using Microsoft::WRL::ComPtr;

	/**
	 * @brief DX12 Root Signature builder and wrapper
	 * Equivalent to VkPipelineLayout in Vulkan
	 */
	class CONCERTO_GRAPHICS_DX12_BACKEND_API Dx12RootSignature
	{
	public:
		struct DescriptorTableInfo
		{
			UINT setIndex; // Which descriptor set this corresponds to
			std::vector<D3D12_DESCRIPTOR_RANGE1> ranges;
			D3D12_SHADER_VISIBILITY visibility;
			bool isSamplerTable = false;
		};

		Dx12RootSignature() = default;

		// Build root signature from descriptor set layouts
		// @param device: D3D12 device
		// @param tables: Descriptor table information
		void Build(ID3D12Device* device, const std::vector<DescriptorTableInfo>& tables);

		// Get the root signature
		ID3D12RootSignature* Get() const noexcept
		{
			return m_rootSignature.Get();
		}

		// Get number of root parameters (descriptor tables)
		UINT GetNumParameters() const noexcept
		{
			return static_cast<UINT>(m_parameterCount);
		}

		// Get the root parameter index for a given descriptor set index
		// Returns UINT_MAX if not found
		UINT GetRootParameterIndex(UINT setIndex, bool isSampler = false) const noexcept;

	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
		struct RootParamMapping
		{
			UINT setIndex;
			UINT rootParamIndex;
			bool isSampler;
		};
		std::vector<RootParamMapping> m_setIndexToRootParam;
		size_t m_parameterCount = 0;
	};
} // namespace cct::gfx::dx12
