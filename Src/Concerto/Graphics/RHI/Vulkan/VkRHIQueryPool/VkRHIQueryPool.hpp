//
// Created by arthur on 10/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VK_RHI_QUERYPOOL_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VK_RHI_QUERYPOOL_HPP

#include <vulkan/vulkan.h>

#include "Concerto/Graphics/RHI/QueryPool.hpp"

namespace cct::gfx::rhi
{
	class VkRHIDevice;

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIQueryPool final : public QueryPool
	{
	public:
		explicit VkRHIQueryPool(VkRHIDevice& device);
		~VkRHIQueryPool() override;

		void BeginFrame(CommandBuffer& cmd) override;
		void EndFrame(CommandBuffer& cmd) override;
		float ReadLastFrameMs() const override { return m_lastMs; }

	private:
		VkRHIDevice&  m_device;
		VkQueryPool   m_queryPool       = VK_NULL_HANDLE;
		float         m_timestampPeriod = 1.0F; // nanoseconds per tick
		int           m_frameIndex      = 0;    // double-buffer: 0 or 1
		uint32_t      m_frameCount      = 0;    // counts BeginFrame calls; read only after >= 2
		float         m_lastMs          = 0.0F;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VK_RHI_QUERYPOOL_HPP
