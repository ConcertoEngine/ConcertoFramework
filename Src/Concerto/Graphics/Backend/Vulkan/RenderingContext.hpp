//
// Created by arthur on 21/12/2022.
//

#ifndef CONCERTO_GRAPHICS_RENDERINGCONTEXT_HPP
#define CONCERTO_GRAPHICS_RENDERINGCONTEXT_HPP

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"

namespace cct::gfx::vk
{
	class Instance;
	class PhysicalDevice;
	class Device;
	class Queue;
	class Pipeline;
	class DescriptorPool;
	class RenderPass;
	class Allocator;
	class CommandBuffer;
	class Fence;
	class CommandPool;
	class RenderPass;

	struct RenderingContext
	{
		Instance* instance;
		PhysicalDevice* physicalDevice;
		Device* device;
		UInt32 queueFamilyIndex;
		Queue* queue;
		UInt32 minImageCount;
		UInt32 imageCount;
		VkSampleCountFlagBits MSAASamples;
		CommandBuffer* commandBuffer;
		Fence* fence;
		CommandPool* commandPool;
		RenderPass* renderPass;
	};
} // cct::gfx::vk
#endif //CONCERTO_GRAPHICS_RENDERINGCONTEXT_HPP
