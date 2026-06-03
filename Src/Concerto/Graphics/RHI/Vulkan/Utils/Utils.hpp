//
// Created by arthur on 15/05/2024.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_UTILS_HPP
#define CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_UTILS_HPP

#include "Concerto/Graphics/Core/ShaderModule/ShaderModule.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include <vulkan/vulkan_core.h>

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Converters
	{
	public:
		static constexpr inline VkFormat ToVulkan(PixelFormat pixelFormat);
		static constexpr inline PixelFormat FromVulkan(VkFormat format);
		static constexpr inline VkAttachmentLoadOp ToVulkan(AttachmentLoadOp loadOp);
		static constexpr inline VkAttachmentStoreOp ToVulkan(AttachmentStoreOp storeOp);
		static constexpr inline VkImageLayout ToVulkan(ImageLayout layout);
		static constexpr inline VkBufferUsageFlags ToVulkan(BufferUsageFlags usage);
		static constexpr inline VkShaderStageFlags ToVulkan(EnumFlags<cct::gfx::ShaderStage> stageFlags);
		static constexpr inline VkDescriptorType ToVulkan(cct::gfx::ShaderBindingType bindingType);
		static constexpr inline VkFormat ToVulkan(rhi::VertexAttributeFormat format);

		template<typename T>
		static constexpr inline VkFlags ToVulkan(UInt32 flags);

		static constexpr inline VkFlags ToVulkan(PipelineStageFlags flags);
		static constexpr inline VkFlags ToVulkan(MemoryAccessFlags flags);
	};
} // namespace cct::gfx::rhi

#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.inl"
#endif // CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_UTILS_HPP
