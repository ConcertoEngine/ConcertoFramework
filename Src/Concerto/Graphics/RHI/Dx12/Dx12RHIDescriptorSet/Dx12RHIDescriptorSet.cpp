//
// Created by arthur on 25/10/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSet/Dx12RHIDescriptorSet.hpp"

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Dx12/Dx12Helpers.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIBuffer/Dx12RHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSetLayout/Dx12RHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHITexture/Dx12RHITexture.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"

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
		else if (bindingType == cct::gfx::ShaderBindingType::ReadOnlyStorageBuffer ||
				 bindingType == cct::gfx::ShaderBindingType::StorageBuffer)
		{
			const UINT byteSize = (range > 0) ? range : dx12Buffer.GetSize();
			const UINT firstElement = offset / 4;
			const UINT numElements = byteSize / 4;
			if (bindingType == cct::gfx::ShaderBindingType::ReadOnlyStorageBuffer)
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
				srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Buffer.FirstElement = firstElement;
				srvDesc.Buffer.NumElements = numElements;
				srvDesc.Buffer.StructureByteStride = 0;
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
				m_device->Get()->CreateShaderResourceView(dx12Buffer.GetResource(), &srvDesc, dstHandle);
			}
			else
			{
				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
				uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
				uavDesc.Buffer.FirstElement = firstElement;
				uavDesc.Buffer.NumElements = numElements;
				uavDesc.Buffer.StructureByteStride = 0;
				uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
				m_device->Get()->CreateUnorderedAccessView(dx12Buffer.GetResource(), nullptr, &uavDesc, dstHandle);
			}
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
			const bool bIsSampler = (b.descriptorType == cct::gfx::ShaderBindingType::Sampler ||
									 b.descriptorType == cct::gfx::ShaderBindingType::CombinedImageSampler);
			if (b.binding == binding)
			{
				isSampler = bIsSampler;
				break;
			}
			// Sampler bindings contribute one SRV to gpuRange and one to samplerRange
			if (bIsSampler)
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
		srvDesc.Format = dx12Texture.GetFormat();
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
			samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			m_device->Get()->CreateSampler(&samplerDesc, samplerHandle);
		}
	}

	void Dx12RHIDescriptorSet::BindStorageImage(UInt32 binding, const Texture& texture)
	{
		const auto& dx12Texture = Cast<const Dx12RHITexture&>(texture);
		const auto& bindings = m_layout->GetBindings();

		UINT descriptorOffset = 0;
		for (const auto& b : bindings)
		{
			if (b.binding == binding)
				break;
			descriptorOffset += b.descriptorCount;
		}

		auto dstHandle = m_gpuRange[descriptorOffset].cpuHandle;

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.Format = dx12Texture.GetFormat();
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		uavDesc.Texture2D.PlaneSlice = 0;
		m_device->Get()->CreateUnorderedAccessView(dx12Texture.GetResource(), nullptr, &uavDesc, dstHandle);
	}
} // namespace cct::gfx::rhi
