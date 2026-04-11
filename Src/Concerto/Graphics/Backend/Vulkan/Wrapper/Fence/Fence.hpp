//
// Created by arthur on 15/06/22.
//

#ifndef CONCERTO_GRAPHICS_FENCE_HPP
#define CONCERTO_GRAPHICS_FENCE_HPP

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API Fence : public Object<VkFence>
	{
	public:
		Fence() = default;
		explicit Fence(Device& device, bool signaled = true);
		~Fence() override;

		Fence(Fence&&) = default;
		Fence(const Fence&) = delete;

		Fence& operator=(Fence&&) = default;
		Fence& operator=(const Fence&) = delete;

		VkResult Create(Device& device, bool signaled = true);

		void Wait(UInt64 timeout) const;
		void Reset() const;
	};
} // namespace cct::gfx::vk

#endif //CONCERTO_GRAPHICS_FENCE_HPP
