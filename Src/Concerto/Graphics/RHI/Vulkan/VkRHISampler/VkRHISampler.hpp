//
// Created by arthur on 05/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHISAMPLER_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHISAMPLER_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Sampler/Sampler.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Sampler/Sampler.hpp"

namespace cct::gfx::vk
{
	class Device;
}

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHISampler : public rhi::Sampler, public vk::Sampler
	{
	public:
		VkRHISampler(vk::Device& device, SamplerFilter minFilter, SamplerFilter magFilter, SamplerAddressMode addressMode);
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHISAMPLER_HPP
