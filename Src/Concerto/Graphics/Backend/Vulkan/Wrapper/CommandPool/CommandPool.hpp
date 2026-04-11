//
// Created by arthur on 14/06/22.
//

#ifndef CONCERTO_GRAPHICS_COMMANDPOOL_HPP
#define CONCERTO_GRAPHICS_COMMANDPOOL_HPP



#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"

namespace cct::gfx::vk
{
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API CommandPool : public Object<VkCommandPool>
	{
	public:
		CommandPool(Device& device, UInt32 queueFamily);
		~CommandPool() override;
		CommandPool(CommandPool&&) = default;
		CommandPool(const CommandPool&) = delete;

		CommandPool& operator=(CommandPool&&) = default;
		CommandPool& operator=(const CommandPool&) = delete;

		void Reset() const;
		CommandBuffer AllocateCommandBuffer(VkCommandBufferLevel level);
	private:
		UInt32 m_queueFamily;
	};
}

#endif //CONCERTO_GRAPHICS_COMMANDPOOL_HPP
