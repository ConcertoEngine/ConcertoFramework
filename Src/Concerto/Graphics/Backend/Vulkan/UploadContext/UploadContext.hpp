//
// Created by arthu on 14/09/2022.
//

#ifndef CONCERTO_GRAPHICS_UPLOADCONTEXT_HPP
#define CONCERTO_GRAPHICS_UPLOADCONTEXT_HPP

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandPool/CommandPool.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Fence/Fence.hpp"

namespace cct::gfx::vk
{
	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API UploadContext
	{
	public:
		UploadContext() = delete;

		UploadContext(Device& device, UInt32 queueFamily);

		~UploadContext() = default;

		UploadContext(UploadContext&&) = default;

		UploadContext(const UploadContext&) = delete;

		UploadContext& operator=(UploadContext&&) = default;

		UploadContext& operator=(const UploadContext&) = delete;

		void ReserveSecondaryCommandBuffers(UInt32 count);
		CommandBuffer* AcquireSecondaryCommandBuffer();

		void ExecuteSecondaryCommandBuffers();

		Fence m_uploadFence;
		CommandPool m_commandPool;
		CommandBuffer m_commandBuffer;
	private:
		std::size_t m_nextSecondaryCommandBuffer;
		std::vector<CommandBuffer> m_secondCommandBuffers;
	};
}

#endif //CONCERTO_GRAPHICS_UPLOADCONTEXT_HPP
