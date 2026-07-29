//
// Created by arthur on 25/10/2025.
//

#pragma once

#include <d3d12.h>
#include <format>
#include <stdexcept>
#include <string>

#include "Concerto/Graphics/Core/ShaderTypes.hpp"

namespace cct::gfx::dx12
{
	// HRESULT error checking helper
	inline void ThrowIfFailed(HRESULT hr, const char* message = "DirectX 12 operation failed")
	{
		if (FAILED(hr))
		{
			throw std::runtime_error(std::format("{} (HRESULT: 0x{:08X})", message, static_cast<unsigned int>(hr)));
		}
	}

	// Descriptor range helper
	inline D3D12_DESCRIPTOR_RANGE1 MakeDescriptorRange1(
		D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
		UINT numDescriptors,
		UINT baseShaderRegister,
		UINT registerSpace = 0,
		D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
		UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
	{
		D3D12_DESCRIPTOR_RANGE1 range{};
		range.RangeType = rangeType;
		range.NumDescriptors = numDescriptors;
		range.BaseShaderRegister = baseShaderRegister;
		range.RegisterSpace = registerSpace;
		range.Flags = flags;
		range.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
		return range;
	}

	// Root parameter helper
	inline D3D12_ROOT_PARAMETER1 MakeRootDescriptorTable(
		const D3D12_DESCRIPTOR_RANGE1* pDescriptorRanges,
		UINT numDescriptorRanges,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		D3D12_ROOT_PARAMETER1 param{};
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
		param.DescriptorTable.pDescriptorRanges = pDescriptorRanges;
		param.ShaderVisibility = visibility;
		return param;
	}

	// Convert Core ShaderStageFlags to DX12 visibility
	inline D3D12_SHADER_VISIBILITY ShaderStageToVisibility(cct::gfx::ShaderStageFlags stageFlags)
	{
		using cct::gfx::ShaderStage;

		const bool hasVertex = stageFlags.Contains(ShaderStage::Vertex);
		const bool hasFragment = stageFlags.Contains(ShaderStage::Fragment);
		const bool hasCompute = stageFlags.Contains(ShaderStage::Compute);

		if (hasCompute && !hasVertex && !hasFragment)
			return D3D12_SHADER_VISIBILITY_ALL; // Compute doesn't have specific visibility

		if (hasVertex && !hasFragment)
			return D3D12_SHADER_VISIBILITY_VERTEX;

		if (hasFragment && !hasVertex)
			return D3D12_SHADER_VISIBILITY_PIXEL;

		// Multiple stages or all stages
		return D3D12_SHADER_VISIBILITY_ALL;
	}

	// Convert Core ShaderBindingType to DX12 descriptor range type
	inline D3D12_DESCRIPTOR_RANGE_TYPE ShaderBindingTypeToDx12RangeType(cct::gfx::ShaderBindingType bindingType)
	{
		using cct::gfx::ShaderBindingType;

		switch (bindingType)
		{
			case ShaderBindingType::UniformBuffer:
				return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			case ShaderBindingType::StorageBuffer:
				return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			case ShaderBindingType::ReadOnlyStorageBuffer:
				return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case ShaderBindingType::Texture:
				return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case ShaderBindingType::StorageImage:
				return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			case ShaderBindingType::Sampler:
				return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			default:
				return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		}
	}
} // namespace cct::gfx::dx12
