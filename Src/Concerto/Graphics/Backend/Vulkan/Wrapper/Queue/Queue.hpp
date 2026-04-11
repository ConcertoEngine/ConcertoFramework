//
// Created by arthur on 13/09/2022.
//

#ifndef CONCERTO_GRAPHICS_QUEUE_HPP
#define CONCERTO_GRAPHICS_QUEUE_HPP

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;
	class SwapChain;
	class FrameData;
	class CommandBuffer;
	class Semaphore;
	class Fence;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API Queue : public Object<VkQueue>
	{
	 public:
		enum class Type
		{
			Compute,  /**< Compute queue for parallel computation. */
			Graphics, /**< Graphics queue for rendering operations. */
			Transfer  /**< Transfer queue for memory operations. */
		};

		Queue();
		Queue(Device& device, UInt32 queueFamilyIndex);
		~Queue() override = default;

		Queue(Queue&&) = default;
		Queue(const Queue&) = delete;

		Queue& operator=(Queue&&) = default;
		Queue& operator=(const Queue&) = delete;

		VkResult Create(Device& device, UInt32 queueFamilyIndex);
		
		[[nodiscard]] UInt32 GetFamilyIndex() const;

		void Submit(const CommandBuffer& commandBuffer, const Semaphore* presentSemaphore, const Semaphore* renderSemaphore, const Fence& renderFence) const;
		bool Present(const Semaphore& renderSemaphore, SwapChain& swapchain, UInt32 swapchainImageIndex) const;
		void WaitIdle() const;

	 private:
		UInt32 m_queueFamilyIndex;
	};
} // namespace cct::gfx::vk

#endif //CONCERTO_GRAPHICS_QUEUE_HPP
