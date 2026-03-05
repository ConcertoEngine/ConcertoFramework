//
// Created by arthur on 15/05/2024.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_UTILS_HPP
#define CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_UTILS_HPP

#include <vulkan/vulkan_core.h>
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Converters
	{
	public:
		static constexpr inline VkFormat ToVulkan(PixelFormat pixelFormat);
		static constexpr inline VkAttachmentLoadOp ToVulkan(AttachmentLoadOp loadOp);
		static constexpr inline VkAttachmentStoreOp ToVulkan(AttachmentStoreOp storeOp);
		static constexpr inline VkImageLayout ToVulkan(ImageLayout layout);
		static constexpr inline VkBufferUsageFlags ToVulkan(BufferUsageFlags usage);
		static constexpr inline VkFilter ToVulkan(SamplerFilter filter);
		static constexpr inline VkSamplerAddressMode ToVulkan(SamplerAddressMode mode);
		static constexpr inline VkImageUsageFlags ToVulkan(TextureUsageFlags usage);
		static constexpr inline VkDescriptorType ToVulkan(DescriptorType type);
		static constexpr inline VkShaderStageFlags ToVulkan(ShaderStageFlags stages);

		template<typename T>
		static constexpr inline VkFlags ToVulkan(UInt32 flags);

	};
}

#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.inl"
#endif //CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_UTILS_HPP