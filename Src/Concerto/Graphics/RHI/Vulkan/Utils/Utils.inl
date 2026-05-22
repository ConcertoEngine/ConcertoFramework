//
// Created by arthur on 15/05/2024.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_UTILS_INL
#define CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_UTILS_INL

#include <vulkan/vulkan_core.h>
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"

namespace cct::gfx::rhi
{
	constexpr VkFormat Converters::ToVulkan(PixelFormat pixelFormat)
	{
		switch (pixelFormat)
		{
		case PixelFormat::R8uNorm:
			return VK_FORMAT_R8_UNORM;
		case PixelFormat::R8ui:
			return VK_FORMAT_R8_UINT;
		case PixelFormat::RG8uNorm:
			return VK_FORMAT_R8G8_UNORM;
		case PixelFormat::RG8ui:
			return VK_FORMAT_R8G8_UINT;
		case PixelFormat::RGB8uNorm:
			return VK_FORMAT_R8G8B8_UNORM;
		case PixelFormat::RGB8ui:
			return VK_FORMAT_R8G8B8_UINT;
		case PixelFormat::BGRuNorm:
			return VK_FORMAT_B8G8R8_UNORM;
		case PixelFormat::BGRui:
			return VK_FORMAT_B8G8R8_UINT;
		case PixelFormat::RGBA8uNorm:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case PixelFormat::RGBA8ui:
			return VK_FORMAT_R8G8B8A8_UINT;
		case PixelFormat::BGRA8uNorm:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case PixelFormat::BGRA8ui:
			return VK_FORMAT_B8G8R8A8_UINT;
		case PixelFormat::R8sNorm:
			return VK_FORMAT_R8_SNORM;
		case PixelFormat::R8i:
			return VK_FORMAT_R8_SINT;
		case PixelFormat::R8_SRGB:
			return VK_FORMAT_R8_SRGB;
		case PixelFormat::R8G8sNorm:
			return VK_FORMAT_R8G8_SNORM;
		case PixelFormat::R8G8i:
			return VK_FORMAT_R8G8_SINT;
		case PixelFormat::R8G8_SRGB:
			return VK_FORMAT_R8G8_SRGB;
		case PixelFormat::RGB8sNorm:
			return VK_FORMAT_R8G8B8_SNORM;
		case PixelFormat::RGB8i:
			return VK_FORMAT_R8G8B8_SINT;
		case PixelFormat::RGB8_SRGB:
			return VK_FORMAT_R8G8B8_SRGB;
		case PixelFormat::BGRsNorm:
			return VK_FORMAT_B8G8R8_SNORM;
		case PixelFormat::BGRi:
			return VK_FORMAT_B8G8R8_SINT;
		case PixelFormat::BGR_SRGB:
			return VK_FORMAT_B8G8R8_SRGB;
		case PixelFormat::RGBA8sNorm:
			return VK_FORMAT_R8G8B8A8_SNORM;
		case PixelFormat::RGBA8i:
			return VK_FORMAT_R8G8B8A8_SINT;
		case PixelFormat::RGBA8_SRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case PixelFormat::BGRA8sNorm:
			return VK_FORMAT_B8G8R8A8_SNORM;
		case PixelFormat::BGRA8i:
			return VK_FORMAT_B8G8R8A8_SINT;
		case PixelFormat::BGRA8_SRGB:
			return VK_FORMAT_B8G8R8A8_SRGB;
		case PixelFormat::R16uNorm:
			return VK_FORMAT_R16_UNORM;
		case PixelFormat::R16G16uNorm:
			return VK_FORMAT_R16G16_UNORM;
		case PixelFormat::R16ui:
			return VK_FORMAT_R16_UINT;
		case PixelFormat::R16G16ui:
			return VK_FORMAT_R16G16_UINT;
		case PixelFormat::RGBuNorm:
			return VK_FORMAT_R16G16B16_UNORM;
		case PixelFormat::RGBui:
			return VK_FORMAT_R16G16B16_UINT;
		case PixelFormat::RGBA16uNorm:
			return VK_FORMAT_R16G16B16A16_UNORM;
		case PixelFormat::RGBA16ui:
			return VK_FORMAT_R16G16B16A16_UINT;
		case PixelFormat::R16sNorm:
			return VK_FORMAT_R16_SNORM;
		case PixelFormat::R16i:
			return VK_FORMAT_R16_SINT;
		case PixelFormat::R16G16sNorm:
			return VK_FORMAT_R16G16_SNORM;
		case PixelFormat::R16G16i:
			return VK_FORMAT_R16G16_SINT;
		case PixelFormat::RGBsNorm:
			return VK_FORMAT_R16G16B16_SNORM;
		case PixelFormat::RGBi:
			return VK_FORMAT_R16G16B16_SINT;
		case PixelFormat::RGBA16sNorm:
			return VK_FORMAT_R16G16B16A16_SNORM;
		case PixelFormat::RGBA16i:
			return VK_FORMAT_R16G16B16A16_SINT;
		case PixelFormat::R16f:
			return VK_FORMAT_R16_SFLOAT;
		case PixelFormat::R16G16f:
			return VK_FORMAT_R16G16_SFLOAT;
		case PixelFormat::RGBf:
			return VK_FORMAT_R16G16B16_SFLOAT;
		case PixelFormat::RGBA16f:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case PixelFormat::R32ui:
			return VK_FORMAT_R32_UINT;
		case PixelFormat::RG32u:
			return VK_FORMAT_R32G32_UINT;
		case PixelFormat::RGB32ui:
			return VK_FORMAT_R32G32B32_UINT;
		case PixelFormat::RGBA32ui:
			return VK_FORMAT_R32G32B32A32_UINT;
		case PixelFormat::R32i:
			return VK_FORMAT_R32_SINT;
		case PixelFormat::RG32i:
			return VK_FORMAT_R32G32_SINT;
		case PixelFormat::RGB32i:
			return VK_FORMAT_R32G32B32_SINT;
		case PixelFormat::RGBA32i:
			return VK_FORMAT_R32G32B32A32_SINT;
		case PixelFormat::R32f:
			return VK_FORMAT_R32_SFLOAT;
		case PixelFormat::RG32f:
			return VK_FORMAT_R32G32_SFLOAT;
		case PixelFormat::RGB32f:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case PixelFormat::RGBA32f:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case PixelFormat::R64ui:
			return VK_FORMAT_R64_UINT;
		case PixelFormat::RG64ui:
			return VK_FORMAT_R64G64_UINT;
		case PixelFormat::RGB64ui:
			return VK_FORMAT_R64G64B64_UINT;
		case PixelFormat::RGBA64ui:
			return VK_FORMAT_R64G64B64A64_UINT;
		case PixelFormat::R64i:
			return VK_FORMAT_R64_SINT;
		case PixelFormat::RG64i:
			return VK_FORMAT_R64G64_SINT;
		case PixelFormat::RGB64i:
			return VK_FORMAT_R64G64B64_SINT;
		case PixelFormat::RGBA64i:
			return VK_FORMAT_R64G64B64A64_SINT;
		case PixelFormat::R64f:
			return VK_FORMAT_R64_SFLOAT;
		case PixelFormat::RG64f:
			return VK_FORMAT_R64G64_SFLOAT;
		case PixelFormat::RGB64f:
			return VK_FORMAT_R64G64B64_SFLOAT;
		case PixelFormat::RGBA64f:
			return VK_FORMAT_R64G64B64A64_SFLOAT;
		case PixelFormat::D32f:
			return VK_FORMAT_D32_SFLOAT;
		}
		return {};
	}

	constexpr VkAttachmentLoadOp Converters::ToVulkan(AttachmentLoadOp loadOp)
	{
		switch (loadOp)
		{
		case AttachmentLoadOp::Load:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		case AttachmentLoadOp::Clear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case AttachmentLoadOp::DontCare:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}
		return {};
	}

	constexpr VkAttachmentStoreOp Converters::ToVulkan(AttachmentStoreOp storeOp)
	{
		switch (storeOp)
		{
		case AttachmentStoreOp::Store:
			return VK_ATTACHMENT_STORE_OP_STORE;
		case AttachmentStoreOp::DontCare:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}
		return {};
	}

	constexpr VkImageLayout Converters::ToVulkan(ImageLayout layout)
	{
		switch (layout)
		{
		case ImageLayout::Undefined:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		case ImageLayout::General:
			return VK_IMAGE_LAYOUT_GENERAL;
		case ImageLayout::ColorAttachmentOptimal:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthStencilAttachmentOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthStencilReadOnlyOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		case ImageLayout::ShaderReadOnlyOptimal:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case ImageLayout::TransferSrcOptimal:
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		case ImageLayout::TransferDstOptimal:
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case ImageLayout::Preinitialized:
			return VK_IMAGE_LAYOUT_PREINITIALIZED;
		case ImageLayout::DepthReadOnlyStencilAttachmentOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthAttachmentStencilReadOnlyOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
		case ImageLayout::DepthAttachmentOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthReadOnlyOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
		case ImageLayout::StencilAttachmentOptimal:
			return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::StencilReadOnlyOptimal:
			return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
		case ImageLayout::ReadOnlyOptimal:
			return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		case ImageLayout::AttachmentOptimal:
			return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		case ImageLayout::PresentSrcKhr:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}
		return {};
	}

	constexpr VkBufferUsageFlags Converters::ToVulkan(BufferUsageFlags usage)
	{
		VkBufferUsageFlags vkFlags = 0;
		if (usage.Contains(BufferUsage::Uniform))
			vkFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if (usage.Contains(BufferUsage::Vertex))
			vkFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if (usage.Contains(BufferUsage::Storage))
			vkFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (usage.Contains(BufferUsage::TransferSrc))
			vkFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (usage.Contains(BufferUsage::TransferDst))
			vkFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if (usage.Contains(BufferUsage::Indirect))
			vkFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		if (usage.Contains(BufferUsage::Index))
			vkFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		return vkFlags;
	}

	template <>
	constexpr VkFlags Converters::ToVulkan<VkPipelineStageFlagBits>(UInt32 flags)
	{
		VkFlags vkFlags = 0;
		if (flags & static_cast<UInt32>(PipelineStage::Pipe))
			vkFlags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::DrawIndirect))
			vkFlags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::VertexInput))
			vkFlags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::VertexShader))
			vkFlags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::TessellationControlShader))
			vkFlags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::TessellationEvaluationShader))
			vkFlags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::GeometryShader))
			vkFlags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::FragmentShader))
			vkFlags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::EarlyFragmentTests))
			vkFlags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::LateFragmentTests))
			vkFlags |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::ColorAttachmentOutput))
			vkFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::ComputeShader))
			vkFlags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::Transfer))
			vkFlags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		if (flags & static_cast<UInt32>(PipelineStage::BottomOfPipe))
			vkFlags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		return vkFlags;
	}

	template <>
	constexpr VkFlags Converters::ToVulkan<VkAccessFlagBits>(UInt32 flags)
	{
		VkFlags vkFlags = 0;
		if (flags & static_cast<UInt32>(MemoryAccess::IndirectCommandRead))
			vkFlags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::IndexRead))
			vkFlags |= VK_ACCESS_INDEX_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::VertexAttributeRead))
			vkFlags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::UniformRead))
			vkFlags |= VK_ACCESS_UNIFORM_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::InputAttachmentRead))
			vkFlags |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::ShaderRead))
			vkFlags |= VK_ACCESS_SHADER_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::ShaderWrite))
			vkFlags |= VK_ACCESS_SHADER_WRITE_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::ColorAttachmentRead))
			vkFlags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::ColorAttachmentWrite))
			vkFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::DepthStencilAttachmentRead))
			vkFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::DepthStencilAttachmentWrite))
			vkFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::TransferRead))
			vkFlags |= VK_ACCESS_TRANSFER_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::TransferWrite))
			vkFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::HostRead))
			vkFlags |= VK_ACCESS_HOST_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::HostWrite))
			vkFlags |= VK_ACCESS_HOST_WRITE_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::MemoryRead))
			vkFlags |= VK_ACCESS_MEMORY_READ_BIT;
		if (flags & static_cast<UInt32>(MemoryAccess::MemoryWrite))
			vkFlags |= VK_ACCESS_MEMORY_WRITE_BIT;
		return vkFlags;
	}

	constexpr VkFlags Converters::ToVulkan(PipelineStageFlags flags)
	{
		return ToVulkan<VkPipelineStageFlagBits>(flags.Value());
	}

	constexpr VkFlags Converters::ToVulkan(MemoryAccessFlags flags)
	{
		return ToVulkan<VkAccessFlagBits>(flags.Value());
	}


	constexpr VkShaderStageFlags Converters::ToVulkan(EnumFlags<cct::gfx::ShaderStage> stageFlags)
	{
		VkShaderStageFlags vkFlags = 0;
		
		if (stageFlags.Contains(cct::gfx::ShaderStage::Vertex))
			vkFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		if (stageFlags.Contains(cct::gfx::ShaderStage::Fragment))
			vkFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if (stageFlags.Contains(cct::gfx::ShaderStage::Compute))
			vkFlags |= VK_SHADER_STAGE_COMPUTE_BIT;

		return vkFlags;
	}

	constexpr VkDescriptorType Converters::ToVulkan(cct::gfx::ShaderBindingType bindingType)
	{
		switch (bindingType)
		{
		case cct::gfx::ShaderBindingType::Sampler:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case cct::gfx::ShaderBindingType::StorageBuffer:
		case cct::gfx::ShaderBindingType::ReadOnlyStorageBuffer:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case cct::gfx::ShaderBindingType::Texture:
			return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		case cct::gfx::ShaderBindingType::UniformBuffer:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case cct::gfx::ShaderBindingType::UniformBufferDynamic:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		case cct::gfx::ShaderBindingType::CombinedImageSampler:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		}
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	}

	constexpr VkFormat Converters::ToVulkan(VertexAttributeFormat format)
	{
		switch (format)
		{
		case VertexAttributeFormat::Vec2f:      return VK_FORMAT_R32G32_SFLOAT;
		case VertexAttributeFormat::Vec4f:      return VK_FORMAT_R32G32B32A32_SFLOAT;
		case VertexAttributeFormat::RGBA8Unorm: return VK_FORMAT_R8G8B8A8_UNORM;
		}
		return VK_FORMAT_UNDEFINED;
	}

	constexpr PixelFormat Converters::FromVulkan(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_B8G8R8A8_UNORM: return PixelFormat::BGRA8uNorm;
		case VK_FORMAT_B8G8R8A8_SRGB:  return PixelFormat::BGRA8_SRGB;
		case VK_FORMAT_R8G8B8A8_UNORM: return PixelFormat::RGBA8uNorm;
		case VK_FORMAT_R8G8B8A8_SRGB:  return PixelFormat::RGBA8_SRGB;
		default:                        return PixelFormat::BGRA8uNorm;
		}
	}
}

#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.inl"

#endif //CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_UTILS_INL
