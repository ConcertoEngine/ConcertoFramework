//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_ENUMS_HPP
#define CONCERTO_GRAPHICS_RHI_ENUMS_HPP

#include <vector>

#include <Concerto/Core/EnumFlags/EnumFlags.hpp>
#include <Concerto/Core/Types/Types.hpp>
#include <Concerto/Graphics/Core/PixelFormat.hpp>

namespace cct::gfx::rhi
{
	enum class DeviceType : UInt8
	{
		Other, // Unknown device type
		Integrated, // GPU integrated to the CPU
		Dedicated, // GPU dedicated to the graphics
		Virtual, // Virtual GPU provided by a virtualization system
		Software // CPU software renderer
	};

	enum class ValidationFlags : UInt8
	{
		None = 0,
		// Core validation. Vulkan: VK_LAYER_KHRONOS_validation. D3D12: debug layer.
		Standard = 1 << 0,
		// Vulkan: VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT. No D3D12 equivalent.
		SyncValidation = 1 << 1,
		// Vulkan: VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT. No D3D12 equivalent. Very verbose.
		BestPractices = 1 << 2,
		// Vulkan: VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT.
		// D3D12: ID3D12Debug1::SetEnableGPUBasedValidation. Costly on both, keep it off by default.
		GpuAssisted = 1 << 3,
		// Vulkan: VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT. No D3D12 equivalent.
		DebugPrintf = 1 << 4
	};

	enum class AttachmentLoadOp : UInt8
	{
		Load,
		Clear,
		DontCare
	};

	enum class AttachmentStoreOp : UInt8
	{
		Store,
		DontCare
	};

	enum class ImageLayout : UInt8
	{
		Undefined,
		General,
		ColorAttachmentOptimal,
		DepthStencilAttachmentOptimal,
		DepthStencilReadOnlyOptimal,
		ShaderReadOnlyOptimal,
		TransferSrcOptimal,
		TransferDstOptimal,
		Preinitialized,
		DepthReadOnlyStencilAttachmentOptimal,
		DepthAttachmentStencilReadOnlyOptimal,
		DepthAttachmentOptimal,
		DepthReadOnlyOptimal,
		StencilAttachmentOptimal,
		StencilReadOnlyOptimal,
		ReadOnlyOptimal,
		AttachmentOptimal,
		PresentSrcKhr,
	};

	enum class PipelineStage : UInt32
	{
		Pipe = 0x00000001,
		DrawIndirect = 0x00000002,
		VertexInput = 0x00000004,
		VertexShader = 0x00000008,
		TessellationControlShader = 0x00000010,
		TessellationEvaluationShader = 0x00000020,
		GeometryShader = 0x00000040,
		FragmentShader = 0x00000080,
		EarlyFragmentTests = 0x00000100,
		LateFragmentTests = 0x00000200,
		ColorAttachmentOutput = 0x00000400,
		ComputeShader = 0x00000800,
		Transfer = 0x00001000,
		BottomOfPipe = 0x00002000
	};
	using PipelineStageFlags = cct::EnumFlags<PipelineStage>;

	enum class MemoryAccess : UInt32
	{
		IndirectCommandRead = 0x00000001,
		IndexRead = 0x00000002,
		VertexAttributeRead = 0x00000004,
		UniformRead = 0x00000008,
		InputAttachmentRead = 0x00000010,
		ShaderRead = 0x00000020,
		ShaderWrite = 0x00000040,
		ColorAttachmentRead = 0x00000080,
		ColorAttachmentWrite = 0x00000100,
		DepthStencilAttachmentRead = 0x00000200,
		DepthStencilAttachmentWrite = 0x00000400,
		TransferRead = 0x00000800,
		TransferWrite = 0x00001000,
		HostRead = 0x00002000,
		HostWrite = 0x00004000,
		MemoryRead = 0x00008000,
		MemoryWrite = 0x00010000
	};
	using MemoryAccessFlags = cct::EnumFlags<MemoryAccess>;

	struct AttachmentReference
	{
		UInt32 attachmentIndex;
		ImageLayout imageLayout;
	};

	enum class QueueFamily : UInt8
	{
		Compute, /**< Compute queue for parallel computation. */
		Graphics, /**< Graphics queue for rendering operations. */
		Transfer /**< Transfer queue for memory operations. */
	};

	enum class BufferUsage : UInt32
	{
		Uniform = 0x00000001,
		Vertex = 0x00000002,
		Storage = 0x00000004,
		TransferSrc = 0x00000008,
		TransferDst = 0x00000010,
		Indirect = 0x00000020,
		Index = 0x00000040,
		HostReadback = 0x00000080
	};
	using BufferUsageFlags = cct::EnumFlags<BufferUsage>;

	enum class VertexAttributeFormat : UInt8
	{
		Vec2f,
		Vec4f,
		RGBA8Unorm,
	};

	struct VertexAttribute
	{
		UInt32 location;
		VertexAttributeFormat format;
		UInt32 offset;
	};

	enum class BlendPreset : UInt8
	{
		// Use blendEnable + premultipliedAlpha logic (legacy default).
		Default = 0,
		// src + dst (additive). For premultiplied src: result = src.rgb + dst.rgb.
		Add,
		// src * dst. result.rgb = src.rgb * dst.rgb.
		Multiply,
		// 1 − (1−src)(1−dst). result.rgb = src.rgb + dst.rgb − src.rgb*dst.rgb.
		Screen,
	};

	struct PipelineConfig
	{
		UInt32 vertexStride = 0;
		std::vector<VertexAttribute> vertexAttributes;
		bool blendEnable = false;
		bool premultipliedAlpha = false;
		BlendPreset blendPreset = BlendPreset::Default;
		bool depthTestEnable = true;
		bool depthWriteEnable = true;
	};

	inline std::size_t PadUniformBuffer(std::size_t size, std::size_t minUniformBufferOffsetAlignment)
	{
		if (size > 0)
			return (size + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1);
		return size;
	}
} // namespace cct::gfx::rhi

CCT_ENABLE_ENUM_FLAGS(cct::gfx::rhi::PipelineStage)
CCT_ENABLE_ENUM_FLAGS(cct::gfx::rhi::MemoryAccess)
CCT_ENABLE_ENUM_FLAGS(cct::gfx::rhi::BufferUsage)
CCT_ENABLE_ENUM_FLAGS(cct::gfx::rhi::ValidationFlags)

#endif // CONCERTO_GRAPHICS_RHI_ENUMS_HPP