//
// Created by arthu on 22/08/2022.
//

#include "Concerto/Graphics/Backend/Vulkan/UploadContext/UploadContext.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{
	UploadContext::UploadContext(Device& device, UInt32 queueFamily) :
		m_uploadFence(device, false),
		m_commandPool(device, queueFamily),
		m_commandBuffer(m_commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY)),
		m_nextSecondaryCommandBuffer(0)
	{
#ifdef CCT_ENABLE_OBJECT_DEBUG
		m_commandBuffer.SetDebugName("UploadContextCommandBuffer");
#endif
	}

	void UploadContext::ReserveSecondaryCommandBuffers(UInt32 count)
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();

		m_secondCommandBuffers.reserve(count);
		// count - 1 because we start at 0
		for (UInt32 i = 0; i < count - 1; ++i)
		{
			auto cb = m_commandPool.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY);
#ifdef CCT_ENABLE_OBJECT_DEBUG
			cb.SetDebugName(std::format("UploadContext::m_secondCommandBuffers[{}]", i));
#endif
			m_secondCommandBuffers.emplace_back(std::move(cb));
		}
	}

	CommandBuffer* UploadContext::AcquireSecondaryCommandBuffer()
	{
		if (m_nextSecondaryCommandBuffer >= m_secondCommandBuffers.size())
			return nullptr; // Exhausted
		return &m_secondCommandBuffers[m_nextSecondaryCommandBuffer++];
	}

	void UploadContext::ExecuteSecondaryCommandBuffers()
	{
		m_commandBuffer.ExecuteCommands(m_secondCommandBuffers);
	}
}
