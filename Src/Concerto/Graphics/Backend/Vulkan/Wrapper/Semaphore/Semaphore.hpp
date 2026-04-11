//
// Created by arthur on 15/06/22.
//

#ifndef CONCERTO_GRAPHICS_SEMAPHORE_HPP
#define CONCERTO_GRAPHICS_SEMAPHORE_HPP

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API Semaphore : public Object<VkSemaphore>
	{
	public:
		Semaphore() = default;
		explicit Semaphore(Device& device);
		~Semaphore() override;

		Semaphore(Semaphore&&) = default;
		Semaphore(const Semaphore&) = delete;

		Semaphore& operator=(Semaphore&&) = default;
		Semaphore& operator=(const Semaphore&) = delete;

		VkResult Create(Device& device);

		Result<UInt64, VkResult> GetSemaphoreCounterValue() const;
	};
} // namespace cct::gfx::vk

#endif //CONCERTO_GRAPHICS_SEMAPHORE_HPP
