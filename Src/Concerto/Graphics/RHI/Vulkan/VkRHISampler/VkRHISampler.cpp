//
// Created by arthur on 05/03/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHISampler/VkRHISampler.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::rhi
{
	VkRHISampler::VkRHISampler(vk::Device& device, SamplerFilter minFilter, SamplerFilter magFilter, SamplerAddressMode addressMode)
		: vk::Sampler(device, Converters::ToVulkan(minFilter), Converters::ToVulkan(addressMode))
	{
		(void)magFilter; // vk::Sampler uses a single filter; minFilter drives both min and mag here
	}
}
