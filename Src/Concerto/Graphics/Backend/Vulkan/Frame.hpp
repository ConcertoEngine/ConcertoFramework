//
// Created by arthur on 13/09/2022.
//

#ifndef CONCERTO_GRAPHICS_FRAME_HPP
#define CONCERTO_GRAPHICS_FRAME_HPP

#include <memory>

#include "Concerto/Graphics/Core/Defines.hpp"

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Buffer/Buffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Fence/Fence.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Semaphore/Semaphore.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandPool/CommandPool.hpp"

#define MAX_OBJECTS 1000

namespace cct::gfx::vk
{
	class Device;

	class CONCERTO_GRAPHICS_CORE_API FrameData
	{
	public:
		FrameData() = delete;

		FrameData(Device& device, bool signaled = true);

		~FrameData() = default;

		FrameData(FrameData&&) = default;

		FrameData(FrameData&) = delete;

		FrameData& operator=(FrameData&&) = default;
		FrameData& operator=(FrameData&) = delete;

		Semaphore presentSemaphore, renderSemaphore;
		Fence renderFence;

		CommandPool commandPool;
		CommandBuffer commandBuffer;

		Buffer indirectBuffer;
		bool isResized;
	};
}
#endif //CONCERTO_GRAPHICS_FRAME_HPP
