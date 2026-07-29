//
// Created by arthur on 25/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDESCRIPTORSET_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDESCRIPTORSET_HPP

#include <memory>
#include <vector>

#include "Concerto/Graphics/Backend/Dx12/Dx12DescriptorHeap.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/DescriptorSet.hpp"

namespace cct::gfx::dx12
{
	class Device;
}

namespace cct::gfx::rhi
{
	class DescriptorSetLayout;
	class Buffer;
	class Texture;

	/**
	 * @brief DX12 Descriptor Set
	 * Holds a range of GPU-visible descriptors that can be bound to a command list
	 */
	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIDescriptorSet : public DescriptorSet
	{
	public:
		Dx12RHIDescriptorSet(std::shared_ptr<DescriptorSetLayout> layout,
							 dx12::DescriptorRange gpuRange,
							 dx12::DescriptorRange samplerRange,
							 dx12::Device* device);

		// RHI interface implementation
		void BindBuffer(UInt32 binding, const Buffer& buffer, UInt32 offset = 0, UInt32 range = 0) override;
		void BindTexture(UInt32 binding, const Texture& texture) override;
		void BindStorageImage(UInt32 binding, const Texture& texture) override;
		const std::shared_ptr<DescriptorSetLayout>& GetLayout() const override
		{
			return m_layout;
		}

		// DX12-specific: Get the CBV/SRV/UAV descriptor range
		const dx12::DescriptorRange& GetDescriptorRange() const noexcept
		{
			return m_gpuRange;
		}
		// DX12-specific: Get the sampler descriptor range
		const dx12::DescriptorRange& GetSamplerRange() const noexcept
		{
			return m_samplerRange;
		}
		bool HasSamplers() const noexcept
		{
			return m_samplerRange.baseHandle.IsValid();
		}

	private:
		std::shared_ptr<DescriptorSetLayout> m_layout;
		dx12::DescriptorRange m_gpuRange; // CBV/SRV/UAV heap
		dx12::DescriptorRange m_samplerRange; // Sampler heap
		dx12::Device* m_device;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDESCRIPTORSET_HPP
