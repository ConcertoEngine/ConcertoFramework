//
// Created by arthur on 09/04/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VK_RHI_QUEUE_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VK_RHI_QUEUE_HPP

#include "Concerto/Graphics/RHI/Queue.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Queue/Queue.hpp"

namespace cct::gfx::rhi
{
	class VkRHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIQueue : public rhi::Queue
	{
	public:
		VkRHIQueue(vk::Queue& queue, VkRHIDevice& device);
		~VkRHIQueue() override = default;

		void Submit(CommandBuffer& cmdBuf, Fence* fence = nullptr) override;

	private:
		vk::Queue* m_queue;
		VkRHIDevice* m_device;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VK_RHI_QUEUE_HPP
