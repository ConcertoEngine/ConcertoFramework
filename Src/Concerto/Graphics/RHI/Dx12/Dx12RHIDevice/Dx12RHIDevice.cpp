//
// Created by arthur on 01/09/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDevice/Dx12RHIDevice.hpp"

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Factory/Factory.hpp"
#include "Concerto/Graphics/Core/ShaderModuleLoader/ShaderModuleLoader.hpp"
#include "Concerto/Graphics/Core/Vertex.hpp"
#include "Concerto/Graphics/RHI/BaseMaterialBuilder.hpp"
#include "Concerto/Graphics/RHI/DescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIBuffer/Dx12RHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHICommandPool/Dx12RHICommandPool.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSet/Dx12RHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIDescriptorSetLayout/Dx12RHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFence/Dx12RHIFence.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIFrameBuffer/Dx12RHIFrameBuffer.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIPipeline/Dx12RHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIPipelineLayout/Dx12RHIPipelineLayout.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIQueryPool/Dx12RHIQueryPool.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIQueue/Dx12RHIQueue.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIRenderPass/Dx12RHIRenderPass.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHIShaderModule/Dx12RHIShaderModule.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHISwapChain/Dx12RHISwapChain.hpp"
#include "Concerto/Graphics/RHI/Dx12/Dx12RHITexture/Dx12RHITexture.hpp"
#include "Concerto/Graphics/RHI/Mesh/Mesh.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIDevice::Dx12RHIDevice(ID3D12Device& device, dx12::PhysicalDevice& physicalDevice) :
		rhi::Device(),
		dx12::Device(device, physicalDevice)
	{
	}

	Dx12RHIDevice::~Dx12RHIDevice() = default;

	std::unique_ptr<SwapChain> Dx12RHIDevice::CreateSwapChain(Window& window, PixelFormat pixelFormat, PixelFormat depthPixelFormat)
	{
		auto swapChain = std::make_unique<Dx12RHISwapChain>(*this, window, pixelFormat, depthPixelFormat);
		if (FAILED(swapChain->GetLastResult()))
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Error occured during swapchain creation error={}", swapChain->GetLastResult());
			return nullptr;
		}
		return swapChain;
	}

	std::unique_ptr<RenderPass> Dx12RHIDevice::CreateRenderPass(std::span<RenderPass::Attachment> attachments,
																std::span<RenderPass::SubPassDescription> subPassDescriptions, std::span<RenderPass::SubPassDependency> subPassDependencies)
	{
		return std::make_unique<Dx12RHIRenderPass>(attachments, subPassDescriptions, subPassDependencies);
	}

	std::unique_ptr<FrameBuffer> Dx12RHIDevice::CreateFrameBuffer(UInt32 width, UInt32 height,
																  const RenderPass& renderPass, const std::vector<std::unique_ptr<Texture>>& attachments)
	{
		// Extract render target resources from DX12 textures
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> rtResources;

		for (const auto& attachment : attachments)
		{
			if (auto* dx12Texture = dynamic_cast<const Dx12RHITexture*>(attachment.get()))
			{
				rtResources.push_back(Microsoft::WRL::ComPtr<ID3D12Resource>(dx12Texture->GetResource()));
			}
		}

		return std::make_unique<Dx12RHIFrameBuffer>(width, height, std::move(rtvHandles), std::move(rtResources));
	}

	std::unique_ptr<FrameBuffer> Dx12RHIDevice::CreateFrameBuffer(UInt32 width, UInt32 height,
																  const RenderPass& renderPass, const std::vector<std::unique_ptr<TextureView>>& attachments)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> rtResources;

		return std::make_unique<Dx12RHIFrameBuffer>(width, height, std::move(rtvHandles), std::move(rtResources));
	}

	std::unique_ptr<MaterialBuilder> Dx12RHIDevice::CreateMaterialBuilder(const Vector2u& windowExtent)
	{
		return std::make_unique<BaseMaterialBuilder>(*this, windowExtent);
	}

	std::shared_ptr<Texture> Dx12RHIDevice::CreateTexture(PixelFormat format, Int32 width, Int32 height)
	{
		return std::make_shared<Dx12RHITexture>(*this, format, width, height);
	}

	void Dx12RHIDevice::WaitIdle()
	{
		if (!m_renderQueue)
			return;

		if (!m_uploadFence)
		{
			dx12::Device::Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_uploadFence));
			m_uploadEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			m_uploadFenceValue = 0;
		}

		++m_uploadFenceValue;
		m_renderQueue->Signal(m_uploadFence.Get(), m_uploadFenceValue);
		m_uploadFence->SetEventOnCompletion(m_uploadFenceValue, m_uploadEvent);
		WaitForSingleObject(m_uploadEvent, INFINITE);
	}

	std::unique_ptr<TextureBuilder> Dx12RHIDevice::CreateTextureBuilder()
	{
		return std::make_unique<TextureBuilder>(*this);
	}

	std::unique_ptr<CommandPool> Dx12RHIDevice::CreateCommandPool(rhi::QueueFamily family, CommandBufferUsage usage)
	{
		D3D12_COMMAND_LIST_TYPE type;
		switch (family)
		{
			case rhi::QueueFamily::Graphics:
				type = D3D12_COMMAND_LIST_TYPE_DIRECT;
				break;
			case rhi::QueueFamily::Compute:
				type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
				break;
			case rhi::QueueFamily::Transfer:
				type = D3D12_COMMAND_LIST_TYPE_COPY;
				break;
			default:
				type = D3D12_COMMAND_LIST_TYPE_DIRECT;
				break;
		}
		return std::make_unique<Dx12RHICommandPool>(*this, usage, type);
	}

	std::unique_ptr<Buffer> Dx12RHIDevice::CreateBuffer(rhi::BufferUsageFlags usage, UInt32 allocationSize, bool allowBufferMapping)
	{
		return std::make_unique<Dx12RHIBuffer>(*this, usage, allocationSize, allowBufferMapping);
	}

	std::size_t Dx12RHIDevice::GetMinimumUniformBufferOffsetAlignment() const
	{
		return D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	}

	std::unique_ptr<GpuMesh> Dx12RHIDevice::CreateMesh(const std::string& meshPath, rhi::MaterialBuilder& materialBuilder, const RenderPass& renderPass)
	{
		Mesh mesh(meshPath);
		return mesh.BuildGpuMesh(materialBuilder, renderPass, *this);
	}

	std::shared_ptr<rhi::ShaderModule> Dx12RHIDevice::CreateShaderModule(const std::string& path)
	{
		cct::gfx::ShaderModuleLoader loader;
		auto resolved = loader.ResolveShaderModule(path);
		return std::make_shared<Dx12RHIShaderModule>(std::move(resolved));
	}

	std::shared_ptr<rhi::DescriptorSetLayout> Dx12RHIDevice::CreateDescriptorSetLayout(const std::vector<cct::gfx::DescriptorSetLayoutBinding>& bindings)
	{
		return std::make_shared<Dx12RHIDescriptorSetLayout>(bindings);
	}

	std::shared_ptr<rhi::PipelineLayout> Dx12RHIDevice::CreatePipelineLayout(const std::vector<std::shared_ptr<rhi::DescriptorSetLayout>>& descriptorSetLayouts)
	{
		return std::make_shared<Dx12RHIPipelineLayout>(*this, descriptorSetLayouts);
	}

	std::shared_ptr<rhi::Pipeline> Dx12RHIDevice::CreatePipeline(const rhi::ShaderModule& vertexShader, const rhi::ShaderModule& fragmentShader,
																 const rhi::RenderPass& renderPass, const rhi::PipelineLayout& pipelineLayout,
																 const Vector2u& windowExtent)
	{
		const auto& dx12VertexShader = Cast<const Dx12RHIShaderModule&>(vertexShader);
		const auto& dx12FragmentShader = Cast<const Dx12RHIShaderModule&>(fragmentShader);

		// Build pipeline layout with root signature
		auto pipelineLayoutCopy = std::make_shared<Dx12RHIPipelineLayout>(*this, pipelineLayout.GetDescriptorSetLayouts());

		// Define input layout matching nzsl HLSL/DXIL output semantics
		// nzsl maps vertex inputs at location(N) to TEXCOORD(N)
		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(cct::gfx::Vertex, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(cct::gfx::Vertex, normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(cct::gfx::Vertex, color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(cct::gfx::Vertex, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		// Get render pass formats
		DXGI_FORMAT rtvFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;
		if (auto* dx12RenderPass = dynamic_cast<const Dx12RHIRenderPass*>(&renderPass))
		{
			rtvFormat = dx12RenderPass->GetColorAttachmentFormat();
			dsvFormat = dx12RenderPass->GetDepthAttachmentFormat();
		}

		// Build PSO description
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = {inputLayout, _countof(inputLayout)};
		psoDesc.pRootSignature = pipelineLayoutCopy->GetRootSignature().Get();
		psoDesc.VS = dx12VertexShader.GetD3D12ShaderBytecode();
		psoDesc.PS = dx12FragmentShader.GetD3D12ShaderBytecode();

		// Rasterizer state - match Vulkan config
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
		psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthClipEnable = TRUE;
		psoDesc.RasterizerState.MultisampleEnable = FALSE;
		psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		psoDesc.RasterizerState.ForcedSampleCount = 0;
		psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		// Blend state
		psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
		psoDesc.BlendState.IndependentBlendEnable = FALSE;
		psoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		// Depth stencil state - match Vulkan config (LESS_OR_EQUAL)
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		psoDesc.DepthStencilState.StencilEnable = FALSE;

		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = rtvFormat;
		psoDesc.DSVFormat = dsvFormat;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
		HRESULT hr = dx12::Device::Get()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
		if (FAILED(hr))
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Failed to create DX12 graphics pipeline state HRESULT={}", hr);
			return nullptr;
		}

		return std::make_shared<Dx12RHIPipeline>(std::move(pipelineState), std::move(pipelineLayoutCopy));
	}

	std::unique_ptr<rhi::DescriptorSet> Dx12RHIDevice::CreateDescriptorSet(const rhi::DescriptorSetLayout& layout)
	{
		auto& pool = GetDescriptorPool();
		auto descriptorSet = pool.AllocateDescriptorSet(layout);
		if (!descriptorSet)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Failed to allocate DX12 descriptor set");
			return nullptr;
		}
		return descriptorSet;
	}

	Dx12RHIDescriptorPool& Dx12RHIDevice::GetDescriptorPool()
	{
		if (!m_descriptorPool.has_value())
		{
			m_descriptorPool.emplace();
			m_descriptorPool->Initialize(*this, 1000);
		}
		return m_descriptorPool.value();
	}

	void Dx12RHIDevice::RegisterRenderQueue(ID3D12CommandQueue* queue)
	{
		m_renderQueue = queue;
	}

	void Dx12RHIDevice::ExecuteAndWait(ID3D12GraphicsCommandList* cmdList)
	{
		CCT_ASSERT(m_renderQueue, "ConcertoGraphics: No render queue registered on device");

		if (!m_uploadFence)
		{
			dx12::Device::Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_uploadFence));
			m_uploadEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			m_uploadFenceValue = 0;
		}

		ID3D12CommandList* cmdLists[] = {cmdList};
		m_renderQueue->ExecuteCommandLists(1, cmdLists);

		++m_uploadFenceValue;
		m_renderQueue->Signal(m_uploadFence.Get(), m_uploadFenceValue);
		m_uploadFence->SetEventOnCompletion(m_uploadFenceValue, m_uploadEvent);
		WaitForSingleObject(m_uploadEvent, INFINITE);
	}

	Queue& Dx12RHIDevice::GetQueue(rhi::QueueFamily family)
	{
		if (family != rhi::QueueFamily::Graphics)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: DX12 currently only supports Graphics queue");
		}

		if (!m_graphicsQueue)
		{
			CCT_ASSERT(m_renderQueue, "ConcertoGraphics: Render queue not registered");
			m_graphicsQueue = std::make_unique<Dx12RHIQueue>(m_renderQueue, *this);
		}

		return *m_graphicsQueue;
	}

	std::unique_ptr<Fence> Dx12RHIDevice::CreateFence()
	{
		return std::make_unique<Dx12RHIFence>(*this);
	}

	std::unique_ptr<QueryPool> Dx12RHIDevice::CreateQueryPool()
	{
		return std::make_unique<Dx12RHIQueryPool>(*this);
	}
} // namespace cct::gfx::rhi
