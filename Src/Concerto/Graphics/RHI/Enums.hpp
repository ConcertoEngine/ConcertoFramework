//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_ENUMS_HPP
#define CONCERTO_GRAPHICS_RHI_ENUMS_HPP

#include <type_traits>
#include <Concerto/Core/Types/Types.hpp>
#include <Concerto/Graphics/Core/PixelFormat.hpp>

namespace cct::gfx::rhi
{
	enum class DeviceType : UInt8
	{
		Other,      //Unknown device type
		Integrated, //GPU integrated to the CPU
		Dedicated,  //GPU dedicated to the graphics
		Virtual,    //Virtual GPU provided by a virtualization system
		Software    //CPU software renderer
	};

	enum class ValidationLevel : UInt8
	{
		None = 0,
		Info = 1,
		Warning = 2,
		Error = 3,
		All = 4
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
	using PipelineStageFlags = std::underlying_type_t<PipelineStage>;

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
	using MemoryAccessFlags = std::underlying_type_t<MemoryAccess>;

	struct AttachmentReference
	{
		UInt32 attachmentIndex;
		ImageLayout imageLayout;
	};

	enum class QueueFamily : UInt8
	{
		Compute,  /**< Compute queue for parallel computation. */
		Graphics, /**< Graphics queue for rendering operations. */
		Transfer  /**< Transfer queue for memory operations. */
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
	};
	using BufferUsageFlags = std::underlying_type_t<BufferUsage>;

	enum class SamplerFilter : UInt8
	{
		Nearest,
		Linear
	};

	enum class SamplerAddressMode : UInt8
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder
	};

	enum class TextureUsage : UInt8
	{
		Sampled         = 0x01,
		TransferSrc     = 0x02,
		TransferDst     = 0x04,
		ColorAttachment = 0x08,
	};
	using TextureUsageFlags = std::underlying_type_t<TextureUsage>;

	enum class DescriptorType : UInt8
	{
		UniformBuffer,
		UniformBufferDynamic,
		CombinedImageSampler,
		StorageBuffer
	};

	enum class ShaderStage : UInt16
	{
		Vertex   = 0x01,
		Fragment = 0x02,
		Compute  = 0x04
	};
	using ShaderStageFlags = std::underlying_type_t<ShaderStage>;

	struct DescriptorBinding
	{
		UInt32           binding;
		DescriptorType   type;
		UInt32           count      = 1;
		ShaderStageFlags stageFlags = 0;
	};

	struct DescriptorPoolSize
	{
		DescriptorType type;
		UInt32         count;
	};

	inline std::size_t PadUniformBuffer(std::size_t size, std::size_t minUniformBufferOffsetAlignment)
	{
		if (size > 0)
			return (size + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1);
		return size;
	}
}

#endif //CONCERTO_GRAPHICS_RHI_ENUMS_HPP