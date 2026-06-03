//
// Created by arthur on 10/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_QUERYPOOL_HPP
#define CONCERTO_GRAPHICS_RHI_QUERYPOOL_HPP

#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class CommandBuffer;

	class CONCERTO_GRAPHICS_RHI_BASE_API QueryPool
	{
	public:
		virtual ~QueryPool() = default;

		QueryPool(const QueryPool&) = delete;
		QueryPool& operator=(const QueryPool&) = delete;

		// Call before BeginRenderPass: reads previous frame results, records begin timestamp.
		virtual void BeginFrame(CommandBuffer& cmd) = 0;

		// Call after EndRenderPass: records end timestamp; on DX12 also resolves to readback buffer.
		virtual void EndFrame(CommandBuffer& cmd) = 0;

		// Last completed GPU frame time in milliseconds (0 until first full measurement).
		virtual float ReadLastFrameMs() const = 0;

	protected:
		QueryPool() = default;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_QUERYPOOL_HPP
