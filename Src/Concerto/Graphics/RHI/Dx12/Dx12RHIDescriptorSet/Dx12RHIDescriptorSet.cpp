//
// Created by arthur on 25/10/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSet/Dx12RHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSetLayout/Dx12RHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIBuffer/Dx12RHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHITexture/Dx12RHITexture.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Dx12/Dx12Helpers.hpp"

#include <Concerto/Core/Cast.hpp>

namespace cct::gfx::rhi
{
	Dx12RHIDescriptorSet::Dx12RHIDescriptorSet(std::shared_ptr<DescriptorSetLayout> layout,
	                                           dx12::DescriptorRange gpuRange,
	                                           dx12::DescriptorRange samplerRange,
	                                           dx12::Device* device) :
		m_layout(std::move(layout)),
		m_gpuRange(gpuRange),
		m_samplerRange(samplerRange),
		m_device(device)
	{
	}

	void Dx12RHIDescriptorSet::BindBuffer(UInt32 binding, const Buffer& buffer, UInt32 offset, UInt32 range)
	{
		const auto& dx12Buffer = Cast<const Dx12RHIBuffer&>(buffer);
		const auto& bindings = m_layout->GetBindings();

		// Find the offset in the descriptor range for this binding
		// CBV/SRV/UAV descriptors are laid out in order of bindings
		UINT descriptorOffset = 0;
		cct::gfx::ShaderBindingType bindingType = cct::gfx::ShaderBindingType::UniformBuffer;
		for (const auto& b : bindings)
		{
			if (b.binding == binding)
			{
				bindingType = b.descriptorType;
				break;
			}
			// Each binding takes one descriptor slot (samplers add SRV too, counted here)
			descriptorOffset += b.descriptorCount;
		}

		auto dstHandle = m_gpuRange[descriptorOffset].cpuHandle;

		if (bindingType == cct::gfx::ShaderBindingType::UniformBuffer)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
			cbvDesc.BufferLocation = dx12Buffer.GetGPUVirtualAddress() + offset;
			UINT bufSize = (range > 0) ? range : dx12Buffer.GetSize();
			// CBV size must be 256-byte aligned
			cbvDesc.SizeInBytes = (bufSize + 255) & ~255;
			m_device->Get()->CreateConstantBufferView(&cbvDesc, dstHandle);
		}
		else if (bindingType == cct::gfx::ShaderBindingType::ReadOnlyStorageBuffer)
		{
			// StructuredBuffer → SRV
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = 1;
			srvDesc.Buffer.StructureByteStride = (range > 0) ? range : dx12Buffer.GetSize();
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			m_device->Get()->CreateShaderResourceView(dx12Buffer.GetResource(), &srvDesc, dstHandle);
		}
		else if (bindingType == cct::gfx::ShaderBindingType::StorageBuffer)
		{
			// RWStructuredBuffer → UAV
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = 1;
			uavDesc.Buffer.StructureByteStride = (range > 0) ? range : dx12Buffer.GetSize();
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
			m_device->Get()->CreateUnorderedAccessView(dx12Buffer.GetResource(), nullptr, &uavDesc, dstHandle);
		}
	}

	void Dx12RHIDescriptorSet::BindTexture(UInt32 binding, const Texture& texture)
	{
		const auto& dx12Texture = Cast<const Dx12RHITexture&>(texture);
		const auto& bindings = m_layout->GetBindings();

		// Find the SRV offset in the CBV/SRV/UAV range
		// and the sampler offset in the sampler range
		UINT srvOffset = 0;
		UINT samplerOffset = 0;
		bool isSampler = false;
		for (const auto& b : bindings)
		{
			if (b.binding == binding)
			{
				isSampler = (b.descriptorType == cct::gfx::ShaderBindingType::Sampler);
				break;
			}
			// Sampler bindings contribute one SRV to gpuRange and one to samplerRange
			if (b.descriptorType == cct::gfx::ShaderBindingType::Sampler)
			{
				srvOffset += b.descriptorCount;
				samplerOffset += b.descriptorCount;
			}
			else
			{
				srvOffset += b.descriptorCount;
			}
		}

		// Write SRV to CBV/SRV/UAV heap
		auto srvHandle = m_gpuRange[srvOffset].cpuHandle;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // TODO: get from texture
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		m_device->Get()->CreateShaderResourceView(dx12Texture.GetResource(), &srvDesc, srvHandle);

		// Write sampler to sampler heap (for sampler2D bindings)
		if (isSampler && m_samplerRange.baseHandle.IsValid())
		{
			auto samplerHandle = m_samplerRange[samplerOffset].cpuHandle;
			D3D12_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			m_device->Get()->CreateSampler(&samplerDesc, samplerHandle);
		}
	}
}
