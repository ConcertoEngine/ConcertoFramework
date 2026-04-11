//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_APIIMPL_HPP
#define CONCERTO_GRAPHICS_RHI_APIIMPL_HPP

#include <span>
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API APIImpl
	{
	public:
		APIImpl() = default;
		virtual ~APIImpl() = default;

		virtual bool Create(rhi::ValidationLevel validationLevel) = 0;
		virtual std::span<const DeviceInfo> EnumerateDevices() = 0;
		virtual std::unique_ptr<Device> CreateDevice(std::size_t index) = 0;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_APIIMPL_HPP