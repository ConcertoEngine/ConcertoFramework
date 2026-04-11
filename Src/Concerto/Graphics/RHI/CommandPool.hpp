//
// Created by arthur on 03/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_COMMANDPOOL_HPP
#define CONCERTO_GRAPHICS_RHI_COMMANDPOOL_HPP
#include <memory>

#include "Concerto/Graphics/RHI/CommandBuffer.hpp"

namespace cct::gfx::rhi
{
	enum class CommandBufferUsage
	{
		Primary,
		Secondary
	};

	class CONCERTO_GRAPHICS_RHI_BASE_API CommandPool
	{
	public:
		CommandPool(CommandBufferUsage usage);
		virtual ~CommandPool() = default;

		virtual std::unique_ptr<CommandBuffer> AllocateCommandBuffer() = 0;
	protected:
		CommandBufferUsage m_usage;
	};

	inline CommandPool::CommandPool(CommandBufferUsage usage): m_usage(usage)
	{
	}
}

#endif //CONCERTO_GRAPHICS_RHI_COMMANDPOOL_HPP