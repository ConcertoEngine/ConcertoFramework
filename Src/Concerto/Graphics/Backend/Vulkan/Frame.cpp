//
// Created by arthur on 13/09/2022.
//

#include <vk_mem_alloc.h>

#include "Concerto/Graphics/Backend/Vulkan/Frame.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandPool/CommandPool.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{
	FrameData::FrameData(Device& device, bool signaled) :
		presentSemaphore(device),
		renderSemaphore(device),
		renderFence(device, signaled),
		commandPool(device, device.GetQueueFamilyIndex(Queue::Type::Graphics)),
		commandBuffer(commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY)),
		indirectBuffer(device.GetAllocator().AllocateBuffer<VkDrawIndirectCommand>(MAX_OBJECTS, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, true)),
		isResized(false)
	{
	}
}