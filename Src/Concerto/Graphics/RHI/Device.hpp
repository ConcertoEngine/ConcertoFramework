//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_DEVICE_HPP
#define CONCERTO_GRAPHICS_RHI_DEVICE_HPP

#include <string>
#include <memory>
#include <span>
#include <vector>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/TextureBuilder/TextureBuilder.hpp"
#include "Concerto/Graphics/RHI/FrameBuffer.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/RenderPass.hpp"
#include "Concerto/Graphics/RHI/MaterialBuilder.hpp"
#include "Concerto/Graphics/RHI/CommandPool.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/GpuMesh.hpp"
#include "Concerto/Graphics/RHI/Sampler/Sampler.hpp"

namespace cct::gfx
{
	class Window;
}

namespace cct::gfx::rhi
{
	class SwapChain;

	struct DeviceInfo
	{
		std::string name;
		UInt32 vendor;
		DeviceType type;
	};

	class CONCERTO_GRAPHICS_RHI_BASE_API Device
	{
	public:
		Device() = default;
		virtual ~Device() = default;
		virtual std::unique_ptr<SwapChain> CreateSwapChain(Window& window, PixelFormat pixelFormat = PixelFormat::BGRA8_SRGB, PixelFormat depthPixelFormat = PixelFormat::D32f) = 0;
		virtual std::unique_ptr<RenderPass> CreateRenderPass(std::span<RenderPass::Attachment> attachments, std::span<RenderPass::SubPassDescription> subPassDescriptions, std::span<RenderPass::SubPassDependency> subPassDependencies) = 0;
		virtual std::unique_ptr<FrameBuffer> CreateFrameBuffer(UInt32 width, UInt32 height, const RenderPass& renderPass, const std::vector<std::unique_ptr<Texture>>& attachments) = 0;
		virtual std::unique_ptr<FrameBuffer> CreateFrameBuffer(UInt32 width, UInt32 height, const RenderPass& renderPass, const std::vector<std::unique_ptr<TextureView>>& attachments) = 0;
		virtual std::unique_ptr<MaterialBuilder> CreateMaterialBuilder(const Vector2u& windowExtent) = 0;
		virtual std::unique_ptr<TextureBuilder> CreateTextureBuilder() = 0;
		virtual std::unique_ptr<CommandPool> CreateCommandPool(rhi::QueueFamily family) = 0;
		virtual std::unique_ptr<Buffer> CreateBuffer(rhi::BufferUsageFlags usage, UInt32 allocationSize, bool allowBufferMapping) = 0;
		virtual std::size_t GetMinimumUniformBufferOffsetAlignment() const = 0;
		virtual std::unique_ptr<GpuMesh> CreateMesh(const std::string& meshPath, rhi::MaterialBuilder& materialBuilder, const RenderPass& renderPass) = 0;
		virtual void WaitIdle() = 0;
		virtual std::unique_ptr<Sampler> CreateSampler(SamplerFilter minFilter, SamplerFilter magFilter, SamplerAddressMode addressMode) = 0;
		virtual std::unique_ptr<Texture> CreateTexture(PixelFormat format, Int32 width, Int32 height, TextureUsageFlags usage) = 0;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DEVICE_HPP