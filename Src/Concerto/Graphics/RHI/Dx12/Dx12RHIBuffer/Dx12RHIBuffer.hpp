//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIBUFFER_HPP

#include "Concerto/Graphics/Backend/Dx12/Defines.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"

namespace cct::gfx::rhi
{
	class Dx12RHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIBuffer : public rhi::Buffer
	{
	public:
		Dx12RHIBuffer(Dx12RHIDevice& device, rhi::BufferUsageFlags usage, UInt32 size, bool allowMapping);
		~Dx12RHIBuffer() override = default;

		bool CopyTo(const Texture& texture) override;
		bool Map(Byte** data) override;
		void UnMap() override;

		[[nodiscard]] ID3D12Resource* GetResource() const
		{
			return m_resource.Get();
		}
		[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
		{
			return m_gpuAddress;
		}
		[[nodiscard]] UInt32 GetSize() const
		{
			return m_size;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
		Dx12RHIDevice* m_device;
		UInt32 m_size;
		bool m_allowMapping;
		D3D12_GPU_VIRTUAL_ADDRESS m_gpuAddress = 0;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHIBUFFER_HPP
