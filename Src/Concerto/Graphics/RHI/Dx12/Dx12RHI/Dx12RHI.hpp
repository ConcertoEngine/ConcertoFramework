//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_VKRHIBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_VKRHIBUFFER_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Instance/APIImpl.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHI : public rhi::APIImpl
	{
	public:
		Dx12RHI() = default;
		~Dx12RHI() override;

		bool Create(rhi::ValidationLevel validationLevel) override;

		[[nodiscard]] std::span<const rhi::DeviceInfo> EnumerateDevices() override;
		std::unique_ptr<rhi::Device> CreateDevice(std::size_t index) override;

		static void SetLogger(Logger& logger);

	private:
		void* m_factory; // very ugly but this avoids including Windows headers
		std::vector<DeviceInfo> m_deviceInfos;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_VKRHIBUFFER_HPP