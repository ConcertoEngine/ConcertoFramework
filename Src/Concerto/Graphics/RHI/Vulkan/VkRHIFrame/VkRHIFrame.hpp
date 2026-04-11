//
// Created by arthur on 05/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_VKRHIFRAME_HPP
#define CONCERTO_GRAPHICS_RHI_VKRHIFRAME_HPP

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice.hpp"
#include "Concerto/Graphics/RHI/CommandBuffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIFrame final: public rhi::Frame
	{
	public:
		VkRHIFrame();

	};
}

#endif //CONCERTO_GRAPHICS_RHI_VKRHIFRAME_HPP