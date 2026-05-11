//
// Created by arthur on 10/05/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIQueryPool/VkRHIQueryPool.hpp"

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandBuffer/VkRHICommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"

namespace cct::gfx::rhi
{
	VkRHIQueryPool::VkRHIQueryPool(VkRHIDevice& device) :
		m_device(device)
	{
		auto& vkDev = static_cast<vk::Device&>(device);

		VkQueryPoolCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
		info.queryType = VK_QUERY_TYPE_TIMESTAMP;
		info.queryCount = 4; // 2 frames × 2 timestamps

		const VkResult r = vkDev.vkCreateQueryPool(*vkDev.Get(), &info, nullptr, &m_queryPool);
		if (r != VK_SUCCESS)
		{
			CCT_RHI_LOG_WARN("VkRHIQueryPool: vkCreateQueryPool failed ({}); GPU timing unavailable",
							 static_cast<int>(r));
			m_queryPool = VK_NULL_HANDLE;
			return;
		}

		m_timestampPeriod = vkDev.GetPhysicalDevice().GetProperties().limits.timestampPeriod;
	}

	VkRHIQueryPool::~VkRHIQueryPool()
	{
		if (m_queryPool == VK_NULL_HANDLE)
			return;
		auto& vkDev = static_cast<vk::Device&>(m_device);
		vkDev.vkDestroyQueryPool(*vkDev.Get(), m_queryPool, nullptr);
	}

	void VkRHIQueryPool::BeginFrame(CommandBuffer& cmd)
	{
		if (m_queryPool == VK_NULL_HANDLE)
			return;

		auto& vkDev = static_cast<vk::Device&>(m_device);
		auto& vkCmd = static_cast<vk::CommandBuffer&>(static_cast<VkRHICommandBuffer&>(cmd));
		VkCommandBuffer vkHandle = *vkCmd.Get();

		// On the very first call, reset ALL 4 queries so every slot starts in a known state.
		// The spec requires each query to be reset before its first use.
		if (m_frameCount == 0)
		{
			vkDev.vkCmdResetQueryPool(vkHandle, m_queryPool, 0, 4);
		}

		// Read previous frame results only after both slots have been written at least once
		// (m_frameCount >= 2 means slot 0 was written on call 0 and slot 1 on call 1).
		if (m_frameCount >= 2)
		{
			const int prev = 1 - m_frameIndex;
			const auto base = static_cast<uint32_t>(prev * 2);
			uint64_t stamps[2] = {0, 0};
			const VkResult r = vkDev.vkGetQueryPoolResults(
				*vkDev.Get(), m_queryPool,
				base, 2,
				sizeof(stamps), stamps, sizeof(uint64_t),
				VK_QUERY_RESULT_64_BIT);
			if (r == VK_SUCCESS && stamps[1] > stamps[0])
			{
				const double ticks = static_cast<double>(stamps[1] - stamps[0]);
				m_lastMs = static_cast<float>(ticks * static_cast<double>(m_timestampPeriod) / 1e6);
			}
		}

		// Reset current frame's slots and record begin timestamp
		const auto cur = static_cast<uint32_t>(m_frameIndex * 2);
		vkDev.vkCmdResetQueryPool(vkHandle, m_queryPool, cur, 2);
		vkDev.vkCmdWriteTimestamp(vkHandle, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_queryPool, cur);
		++m_frameCount;
	}

	void VkRHIQueryPool::EndFrame(CommandBuffer& cmd)
	{
		if (m_queryPool == VK_NULL_HANDLE)
			return;

		auto& vkDev = static_cast<vk::Device&>(m_device);
		auto& vkCmd = static_cast<vk::CommandBuffer&>(static_cast<VkRHICommandBuffer&>(cmd));
		VkCommandBuffer vkHandle = *vkCmd.Get();

		const auto cur = static_cast<uint32_t>(m_frameIndex * 2);
		vkDev.vkCmdWriteTimestamp(vkHandle, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_queryPool, cur + 1);
		m_frameIndex ^= 1;
	}
} // namespace cct::gfx::rhi
