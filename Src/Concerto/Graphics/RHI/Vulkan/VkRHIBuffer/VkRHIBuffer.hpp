//
// Created by arthur on 03/09/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIBUFFER_HPP
#define CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIBUFFER_HPP

#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Buffer/Buffer.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIBuffer final: public rhi::Buffer, public vk::Buffer
	{
	public:
		VkRHIBuffer(VkRHIDevice& device, rhi::BufferUsageFlags usage, UInt32 allocationSize, bool allowBufferMapping);

		bool CopyTo(const Texture& texture) override;
		bool Map(Byte** data) override;
		void UnMap() override;
	private:
		VkRHIDevice& m_device;
		bool m_allowBufferMapping;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_VULKAN_VKRHIBUFFER_HPP