//
// Created by arthur on 09/04/2026.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIFence/VkRHIFence.hpp"

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"

namespace cct::gfx::rhi
{
	VkRHIFence::VkRHIFence(VkRHIDevice& device) :
		m_fence(device, false)
	{
	}

	void VkRHIFence::Wait()
	{
		m_fence.Wait(UINT64_MAX);
	}

	void VkRHIFence::Reset()
	{
		m_fence.Reset();
	}
} // namespace cct::gfx::rhi
