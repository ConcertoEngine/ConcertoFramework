//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_DEVICE_HPP
#define CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_DEVICE_HPP

#include <optional>

#include <unordered_map>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/UploadContext/UploadContext.hpp"
#include "Concerto/Graphics/Backend/Vulkan/DescriptorAllocator/DescriptorAllocator.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIQueue/VkRHIQueue.hpp"

namespace cct::gfx::rhi
{

	class CONCERTO_GRAPHICS_RHI_BASE_API VkRHIDevice final : public rhi::Device, public vk::Device
	{
	public:
		VkRHIDevice(vk::PhysicalDevice& physicalDevice);
		~VkRHIDevice();

		// overrides
		std::unique_ptr<SwapChain> CreateSwapChain(Window& window, PixelFormat pixelFormat = PixelFormat::BGRA8_SRGB, PixelFormat depthPixelFormat = PixelFormat::D32f) override;
		std::unique_ptr<RenderPass> CreateRenderPass(std::span<RenderPass::Attachment> attachments, std::span<RenderPass::SubPassDescription> subPassDescriptions, std::span<rhi::RenderPass::SubPassDependency> subPassDependencies) override;
		std::unique_ptr<FrameBuffer> CreateFrameBuffer(UInt32 width, UInt32 height, const rhi::RenderPass& renderPass, const std::vector<std::unique_ptr<rhi::Texture>>& attachments) override;
		std::unique_ptr<FrameBuffer> CreateFrameBuffer(UInt32 width, UInt32 height, const RenderPass& renderPass, const std::vector<std::unique_ptr<TextureView>>& attachments) override;
		std::unique_ptr<MaterialBuilder> CreateMaterialBuilder(const Vector2u& windowExtent) override;
		std::unique_ptr<TextureBuilder> CreateTextureBuilder() override;
		std::unique_ptr<CommandPool> CreateCommandPool(rhi::QueueFamily family, CommandBufferUsage usage) override;
		std::unique_ptr<rhi::Buffer> CreateBuffer(rhi::BufferUsageFlags usage, UInt32 allocationSize, bool allowBufferMapping) override;
		std::shared_ptr<rhi::ShaderModule> CreateShaderModule(const std::string& path) override;
		std::shared_ptr<rhi::DescriptorSetLayout> CreateDescriptorSetLayout(const std::vector<cct::gfx::DescriptorSetLayoutBinding>& bindings) override;
		std::shared_ptr<rhi::PipelineLayout> CreatePipelineLayout(const std::vector<std::shared_ptr<rhi::DescriptorSetLayout>>& descriptorSetLayouts) override;
		std::shared_ptr<rhi::Pipeline> CreatePipeline(const rhi::ShaderModule& vertexShader, const rhi::ShaderModule& fragmentShader,
		                                               const rhi::RenderPass& renderPass, const rhi::PipelineLayout& pipelineLayout,
		                                               const Vector2u& windowExtent) override;
		std::shared_ptr<rhi::Pipeline> CreatePipeline(const rhi::ShaderModule& vertexShader, const rhi::ShaderModule& fragmentShader,
		                                               const rhi::RenderPass& renderPass, const rhi::PipelineLayout& pipelineLayout,
		                                               const Vector2u& windowExtent, const rhi::PipelineConfig& config) override;
		std::unique_ptr<rhi::DescriptorSet> CreateDescriptorSet(const rhi::DescriptorSetLayout& layout) override;
		std::size_t GetMinimumUniformBufferOffsetAlignment() const override;
		std::shared_ptr<Texture> CreateTexture(PixelFormat format, Int32 width, Int32 height) override;
		void WaitIdle() override;
		std::unique_ptr<GpuMesh> CreateMesh(const std::string& meshPath, rhi::MaterialBuilder& materialBuilder, const RenderPass& renderPass) override;

		Queue& GetQueue(rhi::QueueFamily family) override;
		std::unique_ptr<Fence> CreateFence() override;
		std::unique_ptr<QueryPool> CreateQueryPool() override;
		std::shared_ptr<Texture> ImportTexture(const rhi::TextureImportInfo& info) override;

		vk::UploadContext& GetUploadContext();
		vk::Instance& GetVkInstance() const;

	private:
		vk::DescriptorAllocator& GetDescriptorAllocator();

		VkSurfaceKHR m_surface;
		std::optional<vk::UploadContext> m_uploadContext;
		std::optional<vk::DescriptorAllocator> m_descriptorAllocator;
		std::unordered_map<rhi::QueueFamily, VkRHIQueue> m_queues;
	};
}

#endif //CONCERTO_GRAPHICS_BACKEND_RHI_VULKAN_DEVICE_HPP