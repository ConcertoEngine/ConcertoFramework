//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDEVICE_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDEVICE_HPP

#include <optional>
#include <memory>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorPool/Dx12RHIDescriptorPool.hpp"

namespace cct::gfx::dx12
{
	class PhysicalDevice;
}

namespace cct::gfx::rhi
{
	class Dx12RHIQueue;

	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIDevice : public rhi::Device, public dx12::Device
	{
	public:
		Dx12RHIDevice(ID3D12Device& device, dx12::PhysicalDevice& physicalDevice);
		~Dx12RHIDevice() override;

		std::unique_ptr<SwapChain> CreateSwapChain(Window& window, PixelFormat pixelFormat = PixelFormat::BGRA8_SRGB, PixelFormat depthPixelFormat = PixelFormat::D32f) override;
		std::unique_ptr<RenderPass> CreateRenderPass(std::span<RenderPass::Attachment> attachments, std::span<RenderPass::SubPassDescription> subPassDescriptions, std::span<RenderPass::SubPassDependency> subPassDependencies) override;
		std::unique_ptr<FrameBuffer> CreateFrameBuffer(UInt32 width, UInt32 height, const RenderPass& renderPass, const std::vector<std::unique_ptr<Texture>>& attachments) override;
		std::unique_ptr<FrameBuffer> CreateFrameBuffer(UInt32 width, UInt32 height, const RenderPass& renderPass, const std::vector<std::unique_ptr<TextureView>>& attachments) override;
		std::unique_ptr<MaterialBuilder> CreateMaterialBuilder(const Vector2u& windowExtent) override;
		std::unique_ptr<TextureBuilder> CreateTextureBuilder() override;
		std::unique_ptr<CommandPool> CreateCommandPool(rhi::QueueFamily family, CommandBufferUsage usage) override;
		std::unique_ptr<Buffer> CreateBuffer(rhi::BufferUsageFlags usage, UInt32 allocationSize, bool allowBufferMapping) override;
		std::shared_ptr<rhi::ShaderModule> CreateShaderModule(const std::string& path,
		                                                       cct::gfx::ShaderStage stageFilter) override;
		std::shared_ptr<rhi::DescriptorSetLayout> CreateDescriptorSetLayout(const std::vector<cct::gfx::DescriptorSetLayoutBinding>& bindings) override;
		std::shared_ptr<rhi::PipelineLayout> CreatePipelineLayout(const std::vector<std::shared_ptr<rhi::DescriptorSetLayout>>& descriptorSetLayouts) override;
		std::shared_ptr<rhi::Pipeline> CreatePipeline(const rhi::ShaderModule& vertexShader, const rhi::ShaderModule& fragmentShader,
		                                               const rhi::RenderPass& renderPass, const rhi::PipelineLayout& pipelineLayout,
		                                               const Vector2u& windowExtent) override;
		std::unique_ptr<rhi::DescriptorSet> CreateDescriptorSet(const rhi::DescriptorSetLayout& layout) override;
		std::size_t GetMinimumUniformBufferOffsetAlignment() const override;
		std::shared_ptr<Texture> CreateTexture(PixelFormat format, Int32 width, Int32 height) override;
		void WaitIdle() override;
		std::unique_ptr<GpuMesh> CreateMesh(const std::string& meshPath, rhi::MaterialBuilder& materialBuilder, const RenderPass& renderPass) override;

		Queue& GetQueue(rhi::QueueFamily family) override;
		std::unique_ptr<Fence> CreateFence() override;
		std::unique_ptr<QueryPool> CreateQueryPool() override;

		Dx12RHIDescriptorPool& GetDescriptorPool();

		void RegisterRenderQueue(ID3D12CommandQueue* queue);
		[[nodiscard]] ID3D12CommandQueue* GetRenderQueue() const { return m_renderQueue; }
		void ExecuteAndWait(ID3D12GraphicsCommandList* cmdList);

	private:
		std::optional<Dx12RHIDescriptorPool> m_descriptorPool;
		ID3D12CommandQueue* m_renderQueue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_uploadFence;
		HANDLE m_uploadEvent = nullptr;
		UInt64 m_uploadFenceValue = 0;
		std::unique_ptr<Dx12RHIQueue> m_graphicsQueue;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DX12_DX12RHIDEVICE_HPP
