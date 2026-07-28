//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_DEVICE_HPP
#define CONCERTO_GRAPHICS_RHI_DEVICE_HPP

#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "Concerto/Graphics/Core/ShaderModule/ShaderModule.hpp"
#include "Concerto/Graphics/Core/Window/NativeWindow.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/CommandPool.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Fence.hpp"
#include "Concerto/Graphics/RHI/FrameBuffer.hpp"
#include "Concerto/Graphics/RHI/GpuMesh.hpp"
#include "Concerto/Graphics/RHI/MaterialBuilder.hpp"
#include "Concerto/Graphics/RHI/QueryPool.hpp"
#include "Concerto/Graphics/RHI/Queue.hpp"
#include "Concerto/Graphics/RHI/RenderPass.hpp"
#include "Concerto/Graphics/RHI/SwapChain.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"
#include "Concerto/Graphics/RHI/TextureBuilder/TextureBuilder.hpp"
#include "Concerto/Graphics/RHI/TextureImportInfo.hpp"

namespace cct::gfx
{
	class Window;
}

namespace cct::gfx::rhi
{
	enum class VendorId : UInt32
	{
		Unknown = 0x0000,
		AMD = 0x1002,
		Intel = 0x8086,
		NVIDIA = 0x10DE,
	};

	struct DeviceInfo
	{
		std::string name;
		VendorId vendor;
		DeviceType type;
	};

	class ShaderModule;
	class DescriptorSetLayout;
	class PipelineLayout;
	class Pipeline;
	class DescriptorSet;

	struct NativeQueueFamilyInfo
	{
		UInt32 index;
		UInt32 count;
		UInt32 flags;
	};

	class CONCERTO_GRAPHICS_RHI_BASE_API Device
	{
	public:
		Device() = default;
		virtual ~Device() = default;
		virtual std::unique_ptr<SwapChain> CreateSwapChain(Window& window, PixelFormat pixelFormat = PixelFormat::BGRA8_SRGB, PixelFormat depthPixelFormat = PixelFormat::D32f) = 0;
		virtual std::unique_ptr<SwapChain> CreateSwapChain(NativeWindow /*nativeWindow*/, UInt32 /*width*/, UInt32 /*height*/, PixelFormat /*pixelFormat*/ = PixelFormat::BGRA8_SRGB, PixelFormat /*depthPixelFormat*/ = PixelFormat::D32f) { return nullptr; }
		virtual std::unique_ptr<RenderPass> CreateRenderPass(std::span<RenderPass::Attachment> attachments, std::span<RenderPass::SubPassDescription> subPassDescriptions, std::span<RenderPass::SubPassDependency> subPassDependencies) = 0;
		virtual std::unique_ptr<FrameBuffer> CreateFrameBuffer(UInt32 width, UInt32 height, const RenderPass& renderPass, const std::vector<std::unique_ptr<Texture>>& attachments) = 0;
		virtual std::unique_ptr<FrameBuffer> CreateFrameBuffer(UInt32 width, UInt32 height, const RenderPass& renderPass, const std::vector<std::unique_ptr<TextureView>>& attachments) = 0;
		virtual std::unique_ptr<MaterialBuilder> CreateMaterialBuilder(const Vector2u& windowExtent) = 0;
		virtual std::unique_ptr<TextureBuilder> CreateTextureBuilder() = 0;
		virtual std::unique_ptr<CommandPool> CreateCommandPool(rhi::QueueFamily family, CommandBufferUsage usage) = 0;
		virtual std::unique_ptr<Buffer> CreateBuffer(rhi::BufferUsageFlags usage, UInt32 allocationSize, bool allowBufferMapping) = 0;
		virtual std::shared_ptr<ShaderModule> CreateShaderModule(const std::string& path,
																 cct::gfx::ShaderStage stageFilter = cct::gfx::ShaderStage::None) = 0;
		virtual std::shared_ptr<ShaderModule> CreateShaderModuleFromSource(std::string_view source, std::string_view label = "<generated>", cct::gfx::ShaderStage stageFilter = cct::gfx::ShaderStage::None)
		{
			return nullptr;
		}
		virtual std::shared_ptr<DescriptorSetLayout> CreateDescriptorSetLayout(const std::vector<cct::gfx::DescriptorSetLayoutBinding>& bindings) = 0;
		virtual std::shared_ptr<PipelineLayout> CreatePipelineLayout(const std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts) = 0;
		virtual std::shared_ptr<Pipeline> CreatePipeline(const ShaderModule& vertexShader, const ShaderModule& fragmentShader,
														 const RenderPass& renderPass, const PipelineLayout& pipelineLayout,
														 const Vector2u& windowExtent) = 0;
		virtual std::shared_ptr<Pipeline> CreatePipeline(const ShaderModule& vertexShader, const ShaderModule& fragmentShader,
														 const RenderPass& renderPass, const PipelineLayout& pipelineLayout,
														 const Vector2u& windowExtent, const PipelineConfig& config)
		{
			return CreatePipeline(vertexShader, fragmentShader, renderPass, pipelineLayout, windowExtent);
		}
		virtual std::shared_ptr<Pipeline> CreateComputePipeline(const ShaderModule& /*computeShader*/, const PipelineLayout& /*pipelineLayout*/)
		{
			return nullptr;
		}
		virtual std::unique_ptr<DescriptorSet> CreateDescriptorSet(const DescriptorSetLayout& layout) = 0;
		virtual std::size_t GetMinimumUniformBufferOffsetAlignment() const = 0;
		virtual std::shared_ptr<Texture> CreateTexture(PixelFormat format, Int32 width, Int32 height) = 0;
		virtual void WaitIdle() = 0;
		virtual std::unique_ptr<GpuMesh> CreateMesh(const std::string& meshPath, rhi::MaterialBuilder& materialBuilder, const RenderPass& renderPass) = 0;

		virtual Queue& GetQueue(rhi::QueueFamily family) = 0;
		virtual std::unique_ptr<Fence> CreateFence() = 0;
		virtual std::unique_ptr<QueryPool> CreateQueryPool()
		{
			return nullptr;
		}
		virtual std::shared_ptr<Texture> ImportTexture(const TextureImportInfo& /*info*/)
		{
			return nullptr;
		}
		// Wraps an already-existing native image (Vulkan: a VkImage passed as void*) as a
		// non-owning sampleable Texture. The caller keeps ownership of the underlying image
		// and must outlive the returned Texture. Used to hand an externally-produced GPU
		// image (e.g. a decoder/converter output) to the viewport without copying.
		virtual std::shared_ptr<Texture> AdoptExternalImage(void* /*nativeImage*/, PixelFormat /*format*/, Int32 /*width*/, Int32 /*height*/, Int32 /*planeIndex*/ = -1)
		{
			return nullptr;
		}
		virtual std::shared_ptr<Texture> CreateStorageTexture(PixelFormat /*format*/, Int32 /*width*/, Int32 /*height*/)
		{
			return nullptr;
		}
		virtual void SetShaderModuleSearchPath(const std::filesystem::path& /*path*/)
		{
		}

		// Native backend handles for external interop (e.g. sharing the device with Qt via
		// QQuickGraphicsDevice::fromDeviceObjects). Returned as opaque pointers so consumers
		// need no backend headers. Non-Vulkan backends return null / 0.
		virtual void* GetNativeInstance() const
		{
			return nullptr;
		}
		virtual void* GetNativePhysicalDevice() const
		{
			return nullptr;
		}
		virtual void* GetNativeDevice() const
		{
			return nullptr;
		}
		virtual UInt32 GetNativeGraphicsQueueFamily() const
		{
			return 0;
		}

		// Extra native interop for hardware video decode (FFmpeg AVVulkanDeviceContext).
		// All opaque / backend-neutral; non-Vulkan backends return empty / false.
		// PFN_vkGetInstanceProcAddr as void* — the loader entry FFmpeg uses to resolve
		// every Vulkan function against our instance.
		virtual void* GetNativeGetInstanceProcAddr() const
		{
			return nullptr;
		}
		// True when the device was created with the KHR H.264 video decode stack enabled.
		virtual bool IsVideoDecodeSupported() const
		{
			return false;
		}
		// Queue family index carrying video-decode capability, or 0xFFFFFFFF if none.
		virtual UInt32 GetNativeVideoDecodeQueueFamily() const
		{
			return 0xFFFFFFFFU;
		}
		// Instance/device extensions actually enabled at creation (FFmpeg validates against these).
		virtual std::vector<std::string> GetEnabledInstanceExtensions() const
		{
			return {};
		}
		virtual std::vector<std::string> GetEnabledDeviceExtensions() const
		{
			return {};
		}
		// All queue families created on the device, with their raw flag bits.
		virtual std::vector<NativeQueueFamilyInfo> GetNativeQueueFamilies() const
		{
			return {};
		}
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DEVICE_HPP