//
// Created by arthur on 15/05/2024.
//

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"

#include <Concerto/Core/Assert.hpp>
#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Image/Image.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"
#include "Concerto/Graphics/Core/ShaderModuleLoader/ShaderModuleLoader.hpp"
#include "Concerto/Graphics/Core/Window/Window.hpp"
#include "Concerto/Graphics/RHI/BaseMaterialBuilder.hpp"
#include "Concerto/Graphics/RHI/Mesh/Mesh.hpp"
#include "Concerto/Graphics/RHI/SwapChain.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIBuffer/VkRHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandPool/VkRHICommandPool.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSet/VkRHIDescriptorSet.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIDescriptorSetLayout/VkRHIDescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIFence/VkRHIFence.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIFrameBuffer/VKRHIFrameBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipeline/VkRHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipelineLayout/VkRHIPipelineLayout.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIQueryPool/VkRHIQueryPool.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIQueue/VkRHIQueue.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIRenderPass/VkRHIRenderPass.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIShaderModule/VkRHIShaderModule.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHISwapChain/VkRHISwapChain.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"

namespace cct::gfx::rhi
{
	VkRHIDevice::VkRHIDevice(vk::PhysicalDevice& physicalDevice) :
		vk::Device(physicalDevice),
		m_surface(nullptr)
	{
	}

	VkRHIDevice::~VkRHIDevice()
	{
		// Ensure all GPU operations are complete before cleanup
		WaitIdle();

		// Explicitly clear all members to ensure proper cleanup order
		// This ensures they are destroyed before the base Device destructor
		m_queues.clear();
		m_descriptorAllocator.reset();
		m_uploadContext.reset();
	}

	std::unique_ptr<SwapChain> VkRHIDevice::CreateSwapChain(Window& window, PixelFormat pixelFormat, PixelFormat depthPixelFormat)
	{
		auto swapChain = std::make_unique<VkRHISwapChain>(*this, window, pixelFormat, depthPixelFormat);
		if (swapChain->GetLastResult() != VK_SUCCESS)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Error occured during swapchain creation error={}", static_cast<Int32>(swapChain->GetLastResult()));
			return nullptr;
		}
		return swapChain;
	}

	std::unique_ptr<RenderPass> VkRHIDevice::CreateRenderPass(std::span<rhi::RenderPass::Attachment> attachments, std::span<rhi::RenderPass::SubPassDescription> subPassDescriptions, std::span<rhi::RenderPass::SubPassDependency> subPassDependencies)
	{
		std::vector<VkAttachmentDescription> vkAttachmentDescriptions;
		std::vector<VkSubpassDescription> vkSubPassDescriptions;
		std::vector<VkSubpassDependency> vkSubPassDependencies;
		vkAttachmentDescriptions.reserve(attachments.size());
		vkSubPassDescriptions.reserve(subPassDescriptions.size());
		vkSubPassDependencies.reserve(subPassDependencies.size());

		for (const auto& attachment : attachments)
		{
			auto& vkAttachment = vkAttachmentDescriptions.emplace_back();
			vkAttachment.format = Converters::ToVulkan(attachment.pixelFormat);
			vkAttachment.loadOp = Converters::ToVulkan(attachment.loadOp);
			vkAttachment.stencilLoadOp = Converters::ToVulkan(attachment.stencilLoadOp);
			vkAttachment.storeOp = Converters::ToVulkan(attachment.storeOp);
			vkAttachment.stencilStoreOp = Converters::ToVulkan(attachment.stencilStoreOp);
			vkAttachment.initialLayout = Converters::ToVulkan(attachment.initialLayout);
			vkAttachment.finalLayout = Converters::ToVulkan(attachment.finalLayout);
			vkAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		}

		std::vector<std::vector<VkAttachmentReference>> vkInputAttachmentReferences(subPassDescriptions.size());
		std::vector<std::vector<VkAttachmentReference>> vkColorAttachmentReferences(subPassDescriptions.size());
		std::vector<VkAttachmentReference> vkDepthStencilAttachment(subPassDescriptions.size());

		std::size_t i = 0;
		for (auto& subPassDescription : subPassDescriptions)
		{
			auto& vkSubPassDesc = vkSubPassDescriptions.emplace_back();

			// Input attachment
			vkInputAttachmentReferences[i].reserve(subPassDescription.inputAttachments.size());
			for (const auto& attachmentReference : subPassDescription.inputAttachments)
				vkInputAttachmentReferences[i].emplace_back(attachmentReference.attachmentIndex, Converters::ToVulkan(attachmentReference.imageLayout));
			vkSubPassDesc.inputAttachmentCount = static_cast<UInt32>(subPassDescription.inputAttachments.size());
			vkSubPassDesc.pInputAttachments = vkInputAttachmentReferences[i].data();

			// Color attachment
			vkColorAttachmentReferences[i].reserve(subPassDescription.colorAttachments.size());
			for (const auto& attachmentReference : subPassDescription.colorAttachments)
				vkColorAttachmentReferences[i].emplace_back(attachmentReference.attachmentIndex, Converters::ToVulkan(attachmentReference.imageLayout));
			vkSubPassDesc.colorAttachmentCount = static_cast<UInt32>(subPassDescription.colorAttachments.size());
			vkSubPassDesc.pColorAttachments = vkColorAttachmentReferences[i].data();

			// DepthStencil attachment
			if (subPassDescription.depthStencilAttachment)
			{
				vkDepthStencilAttachment[i] = {subPassDescription.depthStencilAttachment->attachmentIndex, Converters::ToVulkan(subPassDescription.depthStencilAttachment->imageLayout)};
				vkSubPassDesc.pDepthStencilAttachment = &vkDepthStencilAttachment[i];
			}

			// Preserve attachment
			vkSubPassDesc.preserveAttachmentCount = static_cast<UInt32>(subPassDescription.preserveAttachments.size());
			vkSubPassDesc.pPreserveAttachments = subPassDescription.preserveAttachments.data();
			++i;
		}

		for (const auto& subPassDependency : subPassDependencies)
		{
			auto& vkSubPassDependency = vkSubPassDependencies.emplace_back();
			vkSubPassDependency.srcSubpass = subPassDependency.srcSubPassIndex;
			vkSubPassDependency.srcStageMask = Converters::ToVulkan(subPassDependency.srcStageMask);
			vkSubPassDependency.srcAccessMask = Converters::ToVulkan(subPassDependency.srcAccessFlags);
			vkSubPassDependency.dstSubpass = subPassDependency.dstSubPassIndex;
			vkSubPassDependency.dstStageMask = Converters::ToVulkan(subPassDependency.dstStageMask);
			vkSubPassDependency.dstAccessMask = Converters::ToVulkan(subPassDependency.dstAccessFlags);
		}

		return std::make_unique<VkRHIRenderPass>(*this, vkAttachmentDescriptions, vkSubPassDescriptions, vkSubPassDependencies);
	}

	std::unique_ptr<FrameBuffer> VkRHIDevice::CreateFrameBuffer(UInt32 width, UInt32 height, const rhi::RenderPass& renderPass, const std::vector<std::unique_ptr<rhi::Texture>>& attachments)
	{
		return std::make_unique<VkRHIFrameBuffer>(*this, width, height, Cast<const VkRHIRenderPass&>(renderPass), attachments);
	}

	std::unique_ptr<FrameBuffer> VkRHIDevice::CreateFrameBuffer(UInt32 width, UInt32 height, const RenderPass& renderPass, const std::vector<std::unique_ptr<TextureView>>& attachments)
	{
		return std::make_unique<VkRHIFrameBuffer>(*this, width, height, Cast<const VkRHIRenderPass&>(renderPass), attachments);
	}

	std::unique_ptr<MaterialBuilder> VkRHIDevice::CreateMaterialBuilder(const Vector2u& windowExtent)
	{
		return std::make_unique<BaseMaterialBuilder>(*this, windowExtent);
	}

	std::shared_ptr<Texture> VkRHIDevice::CreateTexture(PixelFormat format, Int32 width, Int32 height)
	{
		const bool isDepth = (format == PixelFormat::D32f);
		const VkImageAspectFlags aspect = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		return std::make_shared<VkRHITexture>(*this, format, width, height, aspect);
	}

	void VkRHIDevice::WaitIdle()
	{
		vk::Device::WaitIdle();
	}

	std::unique_ptr<TextureBuilder> VkRHIDevice::CreateTextureBuilder()
	{
		return std::make_unique<TextureBuilder>(*this);
	}

	std::shared_ptr<rhi::ShaderModule> VkRHIDevice::CreateShaderModule(const std::string& path,
																	   cct::gfx::ShaderStage stageFilter)
	{
		cct::gfx::ShaderModuleLoader loader;
		if (!m_shaderModulePath.empty())
			loader.SetModuleSearchPath(m_shaderModulePath);
		cct::gfx::ShaderModule coreShaderModule = loader.LoadShaderModule(path, stageFilter);
		return std::make_shared<VkRHIShaderModule>(*this, std::move(coreShaderModule));
	}

	std::unique_ptr<CommandPool> VkRHIDevice::CreateCommandPool(rhi::QueueFamily family, CommandBufferUsage usage)
	{
		return std::make_unique<VkRHICommandPool>(*this, family, usage);
	}

	std::unique_ptr<rhi::Buffer> VkRHIDevice::CreateBuffer(rhi::BufferUsageFlags usage, UInt32 allocationSize, bool allowBufferMapping)
	{
		return std::make_unique<VkRHIBuffer>(*this, usage, allocationSize, allowBufferMapping);
	}

	std::size_t VkRHIDevice::GetMinimumUniformBufferOffsetAlignment() const
	{
		return vk::Device::GetPhysicalDevice().GetProperties().limits.minUniformBufferOffsetAlignment;
	}

	std::unique_ptr<GpuMesh> VkRHIDevice::CreateMesh(const std::string& meshPath, rhi::MaterialBuilder& materialBuilder, const RenderPass& renderPass)
	{
		Mesh mesh(meshPath);
		return mesh.BuildGpuMesh(materialBuilder, renderPass, *this);
	}

	vk::UploadContext& VkRHIDevice::GetUploadContext()
	{
		if (m_uploadContext.has_value() == false)
			m_uploadContext.emplace(*this, GetQueueFamilyIndex(vk::Queue::Type::Graphics));
		return m_uploadContext.value();
	}

	vk::DescriptorAllocator& VkRHIDevice::GetDescriptorAllocator()
	{
		if (!m_descriptorAllocator.has_value())
			m_descriptorAllocator.emplace(*this);
		return m_descriptorAllocator.value();
	}

	vk::Instance& VkRHIDevice::GetVkInstance() const
	{
		return vk::Device::GetInstance();
	}

	std::shared_ptr<rhi::DescriptorSetLayout> VkRHIDevice::CreateDescriptorSetLayout(const std::vector<cct::gfx::DescriptorSetLayoutBinding>& bindings)
	{
		// Convert abstract bindings to Vulkan bindings
		std::vector<VkDescriptorSetLayoutBinding> vkBindings;
		vkBindings.reserve(bindings.size());
		for (const auto& binding : bindings)
		{
			VkDescriptorSetLayoutBinding vkBinding = {};
			vkBinding.binding = binding.binding;
			vkBinding.descriptorCount = binding.descriptorCount;
			vkBinding.descriptorType = Converters::ToVulkan(binding.descriptorType);
			vkBinding.stageFlags = Converters::ToVulkan(binding.stageFlags);
			vkBinding.pImmutableSamplers = nullptr;
			vkBindings.push_back(vkBinding);
		}

		return std::make_shared<VkRHIDescriptorSetLayout>(*this, bindings);
	}

	std::shared_ptr<rhi::PipelineLayout> VkRHIDevice::CreatePipelineLayout(const std::vector<std::shared_ptr<rhi::DescriptorSetLayout>>& descriptorSetLayouts)
	{
		return std::make_shared<VkRHIPipelineLayout>(*this, descriptorSetLayouts);
	}

	std::shared_ptr<rhi::Pipeline> VkRHIDevice::CreatePipeline(
		const rhi::ShaderModule& vertexShader, const rhi::ShaderModule& fragmentShader,
		const rhi::RenderPass& renderPass, const rhi::PipelineLayout& pipelineLayout,
		const Vector2u& windowExtent)
	{
		const auto& rhiVertexShader = Cast<const VkRHIShaderModule&>(vertexShader);
		const auto& rhiFragmentShader = Cast<const VkRHIShaderModule&>(fragmentShader);
		const auto& rhiRenderPass = Cast<const VkRHIRenderPass&>(renderPass);
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {
			VulkanInitializer::PipelineShaderStageCreateInfo(static_cast<VkShaderStageFlagBits>(Converters::ToVulkan(vertexShader.GetStage())), *rhiVertexShader.GetVulkanShaderModule().Get()),
			VulkanInitializer::PipelineShaderStageCreateInfo(static_cast<VkShaderStageFlagBits>(Converters::ToVulkan(fragmentShader.GetStage())), *rhiFragmentShader.GetVulkanShaderModule().Get())};

		VkExtent2D extent = {windowExtent.X(), windowExtent.Y()};
		auto pipelineLayoutCopy = std::make_shared<VkRHIPipelineLayout>(*this, pipelineLayout.GetDescriptorSetLayouts());
		vk::PipelineInfo pipelineInfo(shaderStages, extent, *pipelineLayoutCopy);

		// Configure additional pipeline settings
		pipelineInfo.m_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		pipelineInfo.m_rasterizer.cullMode = VK_CULL_MODE_NONE;
		pipelineInfo.m_rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		pipelineInfo.m_colorBlendAttachment = VulkanInitializer::ColorBlendAttachmentState();
		pipelineInfo.m_depthStencil = VulkanInitializer::DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

		auto vkPipeline = std::make_shared<vk::Pipeline>(*this, pipelineInfo, Cast<const vk::RenderPass&>(rhiRenderPass));

		return std::make_shared<VkRHIPipeline>(std::move(vkPipeline), std::move(pipelineLayoutCopy));
	}

	std::shared_ptr<rhi::Pipeline> VkRHIDevice::CreatePipeline(
		const rhi::ShaderModule& vertexShader, const rhi::ShaderModule& fragmentShader,
		const rhi::RenderPass& renderPass, const rhi::PipelineLayout& pipelineLayout,
		const Vector2u& windowExtent, const rhi::PipelineConfig& config)
	{
		const auto& rhiVertexShader = Cast<const VkRHIShaderModule&>(vertexShader);
		const auto& rhiFragmentShader = Cast<const VkRHIShaderModule&>(fragmentShader);
		const auto& rhiRenderPass = Cast<const VkRHIRenderPass&>(renderPass);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {
			VulkanInitializer::PipelineShaderStageCreateInfo(static_cast<VkShaderStageFlagBits>(Converters::ToVulkan(vertexShader.GetStage())), *rhiVertexShader.GetVulkanShaderModule().Get()),
			VulkanInitializer::PipelineShaderStageCreateInfo(static_cast<VkShaderStageFlagBits>(Converters::ToVulkan(fragmentShader.GetStage())), *rhiFragmentShader.GetVulkanShaderModule().Get())};

		VkExtent2D extent = {windowExtent.X(), windowExtent.Y()};
		auto pipelineLayoutCopy = std::make_shared<VkRHIPipelineLayout>(*this, pipelineLayout.GetDescriptorSetLayouts());
		vk::PipelineInfo pipelineInfo(shaderStages, extent, *pipelineLayoutCopy);

		// Build vertex input from PipelineConfig
		VkVertexInputBindingDescription bindingDesc{};
		bindingDesc.binding = 0;
		bindingDesc.stride = config.vertexStride;
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> attrDescs;
		attrDescs.reserve(config.vertexAttributes.size());
		for (const auto& attr : config.vertexAttributes)
		{
			VkVertexInputAttributeDescription vkAttr{};
			vkAttr.location = attr.location;
			vkAttr.binding = 0;
			vkAttr.format = Converters::ToVulkan(attr.format);
			vkAttr.offset = attr.offset;
			attrDescs.push_back(vkAttr);
		}

		pipelineInfo.m_vertexInputInfo.vertexBindingDescriptionCount = config.vertexStride > 0 ? 1 : 0;
		pipelineInfo.m_vertexInputInfo.pVertexBindingDescriptions = config.vertexStride > 0 ? &bindingDesc : nullptr;
		pipelineInfo.m_vertexInputInfo.vertexAttributeDescriptionCount = static_cast<UInt32>(attrDescs.size());
		pipelineInfo.m_vertexInputInfo.pVertexAttributeDescriptions = attrDescs.empty() ? nullptr : attrDescs.data();

		// Blend state
		const VkColorComponentFlags allChannels =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		if (config.blendPreset == rhi::BlendPreset::Add)
		{
			// result = src + dst (additive). Works with premultiplied src out of the box.
			pipelineInfo.m_colorBlendAttachment.blendEnable = VK_TRUE;
			pipelineInfo.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.colorWriteMask = allChannels;
		}
		else if (config.blendPreset == rhi::BlendPreset::Multiply)
		{
			// result.rgb = src.rgb * dst.rgb. (srcFactor=DST_COLOR, dstFactor=ZERO)
			pipelineInfo.m_colorBlendAttachment.blendEnable = VK_TRUE;
			pipelineInfo.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
			pipelineInfo.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			pipelineInfo.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			pipelineInfo.m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.colorWriteMask = allChannels;
		}
		else if (config.blendPreset == rhi::BlendPreset::Screen)
		{
			// result.rgb = src + dst − src*dst. (srcFactor=ONE, dstFactor=ONE_MINUS_SRC_COLOR)
			pipelineInfo.m_colorBlendAttachment.blendEnable = VK_TRUE;
			pipelineInfo.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			pipelineInfo.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			pipelineInfo.m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.colorWriteMask = allChannels;
		}
		else if (config.blendEnable && config.premultipliedAlpha)
		{
			pipelineInfo.m_colorBlendAttachment.blendEnable = VK_TRUE;
			pipelineInfo.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			pipelineInfo.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			pipelineInfo.m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.colorWriteMask = allChannels;
		}
		else if (config.blendEnable)
		{
			pipelineInfo.m_colorBlendAttachment.blendEnable = VK_TRUE;
			pipelineInfo.m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			pipelineInfo.m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			pipelineInfo.m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineInfo.m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			pipelineInfo.m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.m_colorBlendAttachment.colorWriteMask = allChannels;
		}

		pipelineInfo.m_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		pipelineInfo.m_rasterizer.cullMode = VK_CULL_MODE_NONE;
		pipelineInfo.m_rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		pipelineInfo.m_depthStencil = VulkanInitializer::DepthStencilCreateInfo(config.depthTestEnable, config.depthWriteEnable, VK_COMPARE_OP_LESS_OR_EQUAL);

		auto vkPipeline = std::make_shared<vk::Pipeline>(*this, pipelineInfo, Cast<const vk::RenderPass&>(rhiRenderPass));
		return std::make_shared<VkRHIPipeline>(std::move(vkPipeline), std::move(pipelineLayoutCopy));
	}

	std::unique_ptr<rhi::DescriptorSet> VkRHIDevice::CreateDescriptorSet(const rhi::DescriptorSetLayout& layout)
	{
		// Get the Vulkan descriptor set layout
		const auto& vkRhiLayout = Cast<const VkRHIDescriptorSetLayout&>(layout);
		const auto& vkLayout = static_cast<const vk::DescriptorSetLayout&>(vkRhiLayout);

		// Allocate descriptor set from the descriptor allocator
		auto& allocator = GetDescriptorAllocator();
		vk::DescriptorSetPtr vkDescriptorSet;

		if (!allocator.AllocateWithoutCache(vkDescriptorSet, vkLayout))
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Failed to allocate descriptor set");
			return nullptr;
		}

		// Create RHI wrapper
		return std::make_unique<VkRHIDescriptorSet>(vkDescriptorSet, std::make_shared<VkRHIDescriptorSetLayout>(
																		 *this, vkRhiLayout.GetBindings()));
	}
	Queue& VkRHIDevice::GetQueue(rhi::QueueFamily family)
	{
		// Map RHI QueueFamily to backend vk::Queue::Type
		vk::Queue::Type vkQueueType;
		switch (family)
		{
			case rhi::QueueFamily::Graphics:
				vkQueueType = vk::Queue::Type::Graphics;
				break;
			case rhi::QueueFamily::Compute:
				vkQueueType = vk::Queue::Type::Compute;
				break;
			case rhi::QueueFamily::Transfer:
				vkQueueType = vk::Queue::Type::Transfer;
				break;
			default:
				CCT_ASSERT_FALSE("ConcertoGraphics: Unknown QueueFamily");
				return m_queues.begin()->second;
		}

		// Check if queue already exists in the map
		auto it = m_queues.find(family);
		if (it != m_queues.end())
			return it->second;

		// Lazily create and insert the VkRHIQueue
		auto& vkQueue = vk::Device::GetQueue(vkQueueType);
		auto [inserted_it, success] = m_queues.emplace(family, VkRHIQueue(vkQueue, *this));
		CCT_ASSERT(success, "ConcertoGraphics: Failed to insert queue");
		return inserted_it->second;
	}

	std::unique_ptr<Fence> VkRHIDevice::CreateFence()
	{
		return std::make_unique<VkRHIFence>(*this);
	}

	std::shared_ptr<Texture> VkRHIDevice::ImportTexture(const rhi::TextureImportInfo& info)
	{
		switch (info.handleType)
		{
#ifdef CCT_PLATFORM_WINDOWS
			case rhi::ExternalHandleType::D3D11NtHandle:
			{
				const VkExtent2D extent{static_cast<UInt32>(info.width), static_cast<UInt32>(info.height)};
				auto image = vk::Image::ImportFromWin32Handle(
					*this, extent, Converters::ToVulkan(info.format), static_cast<HANDLE>(info.handle));
				if (!image)
				{
					CCT_ASSERT_FALSE("VkRHIDevice::ImportTexture: ImportFromWin32Handle failed");
					return nullptr;
				}
				return std::make_shared<VkRHITexture>(*this, std::move(*image));
			}
#endif
			default:
				CCT_ASSERT_FALSE("VkRHIDevice::ImportTexture: unsupported ExternalHandleType {}",
								 static_cast<UInt32>(info.handleType));
				return nullptr;
		}
	}

	std::unique_ptr<QueryPool> VkRHIDevice::CreateQueryPool()
	{
		return std::make_unique<VkRHIQueryPool>(*this);
	}

} // namespace cct::gfx::rhi
