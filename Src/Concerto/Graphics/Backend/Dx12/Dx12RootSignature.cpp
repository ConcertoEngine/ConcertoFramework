//
// Created by arthur on 25/10/2025.
//

#include "Concerto/Graphics/Backend/Dx12/Dx12RootSignature.hpp"

#include <algorithm>

namespace cct::gfx::dx12
{
	void Dx12RootSignature::Build(ID3D12Device* device, const std::vector<DescriptorTableInfo>& tables, bool computeOnly)
	{
		const D3D12_ROOT_SIGNATURE_FLAGS flags = computeOnly
													 ? D3D12_ROOT_SIGNATURE_FLAG_NONE
													 : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		if (tables.empty())
		{
			// Create an empty root signature
			D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
			rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
			rootSignatureDesc.Desc_1_1.NumParameters = 0;
			rootSignatureDesc.Desc_1_1.pParameters = nullptr;
			rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
			rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
			rootSignatureDesc.Desc_1_1.Flags = flags;

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			ThrowIfFailed(
				D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error),
				"Failed to serialize root signature");

			ThrowIfFailed(
				device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
											IID_PPV_ARGS(&m_rootSignature)),
				"Failed to create root signature");

			m_parameterCount = 0;
			return;
		}

		// Build root parameters
		std::vector<D3D12_ROOT_PARAMETER1> rootParameters;
		rootParameters.reserve(tables.size());

		// Keep descriptor ranges alive during serialization
		std::vector<std::vector<D3D12_DESCRIPTOR_RANGE1>> allRanges;
		allRanges.reserve(tables.size());

		m_setIndexToRootParam.clear();
		m_setIndexToRootParam.reserve(tables.size());

		for (const auto& table : tables)
		{
			// Store ranges
			allRanges.push_back(table.ranges);

			// Create root parameter for this descriptor table
			D3D12_ROOT_PARAMETER1 param{};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(table.ranges.size());
			param.DescriptorTable.pDescriptorRanges = allRanges.back().data();
			param.ShaderVisibility = table.visibility;

			// Record mapping
			m_setIndexToRootParam.push_back({table.setIndex, static_cast<UINT>(rootParameters.size()), table.isSamplerTable});

			rootParameters.push_back(param);
		}

		// Create root signature description
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
		rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSignatureDesc.Desc_1_1.NumParameters = static_cast<UINT>(rootParameters.size());
		rootSignatureDesc.Desc_1_1.pParameters = rootParameters.data();
		rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
		rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
		rootSignatureDesc.Desc_1_1.Flags = flags;

		// Serialize and create
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);

		if (FAILED(hr))
		{
			if (error)
			{
				std::string errorMsg = static_cast<const char*>(error->GetBufferPointer());
				throw std::runtime_error("Failed to serialize root signature: " + errorMsg);
			}
			ThrowIfFailed(hr, "Failed to serialize root signature");
		}

		ThrowIfFailed(
			device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
										IID_PPV_ARGS(&m_rootSignature)),
			"Failed to create root signature");

		m_parameterCount = rootParameters.size();
	}

	UINT Dx12RootSignature::GetRootParameterIndex(UINT setIndex, bool isSampler) const noexcept
	{
		auto it = std::find_if(m_setIndexToRootParam.begin(), m_setIndexToRootParam.end(),
							   [setIndex, isSampler](const auto& m)
							   { return m.setIndex == setIndex && m.isSampler == isSampler; });

		if (it != m_setIndexToRootParam.end())
			return it->rootParamIndex;

		return UINT_MAX; // Not found
	}
} // namespace cct::gfx::dx12
