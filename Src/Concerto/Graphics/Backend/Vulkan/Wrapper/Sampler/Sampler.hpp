//
// Created by arthur on 24/10/2022.
//

#ifndef CONCERTO_GRAPHICS_SAMPLER_HPP
#define CONCERTO_GRAPHICS_SAMPLER_HPP



#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API Sampler : public Object<VkSampler>
	{
	public:
		Sampler() = default;
		Sampler(Device& device, VkFilter filter, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
		~Sampler() override;

		Sampler(Sampler&&) noexcept = default;
		Sampler(const Sampler&) = delete;

		Sampler& operator=(Sampler&&) noexcept = default;
		Sampler& operator=(const Sampler&) = delete;

		VkResult Create(Device& device, VkFilter filter, VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
	};
}


#endif //CONCERTO_GRAPHICS_SAMPLER_HPP
