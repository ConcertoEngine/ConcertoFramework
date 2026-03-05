//
// Created by arthur on 15/05/2024.
//

#include <Concerto/Core/Cast.hpp>

#include "Concerto/Graphics/Core/Window/Window.hpp"
#include "Concerto/Graphics/RHI/SwapChain.hpp"

#include "Concerto/Graphics/RHI/Vulkan/VkRHIDevice/VkRHIDevice.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHISwapChain/VkRHISwapChain.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIRenderPass/VkRHIRenderPass.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIFrameBuffer/VKRHIFrameBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/Utils/Utils.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIBuffer/VkRHIBuffer.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHICommandPool/VkRHICommandPool.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIMaterialBuilder/VkRHIMaterialBuilder.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHITextureBuilder/VkRHITextureBuilder.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHISampler/VkRHISampler.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHITexture/VKRHITexture.hpp"

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"

namespace cct::gfx::rhi
{
	VkRHIDevice::VkRHIDevice(vk::PhysicalDevice& physicalDevice) :
		vk::Device(physicalDevice),
		m_surface(nullptr)
	{
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
			vkAttachment.finalLayout = Converters::ToVulkan(attachment.finalLayout);
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

			//Input attachment
			vkInputAttachmentReferences[i].reserve(subPassDescription.inputAttachments.size());
			for (const auto& attachmentReference : subPassDescription.inputAttachments)
				vkInputAttachmentReferences[i].emplace_back(attachmentReference.attachmentIndex, Converters::ToVulkan(attachmentReference.imageLayout));
			vkSubPassDesc.inputAttachmentCount = static_cast<UInt32>(subPassDescription.inputAttachments.size());
			vkSubPassDesc.pInputAttachments = vkInputAttachmentReferences[i].data();

			//Color attachment
			vkColorAttachmentReferences[i].reserve(subPassDescription.colorAttachments.size());
			for (const auto& attachmentReference : subPassDescription.colorAttachments)
				vkColorAttachmentReferences[i].emplace_back(attachmentReference.attachmentIndex, Converters::ToVulkan(attachmentReference.imageLayout));
			vkSubPassDesc.colorAttachmentCount = static_cast<UInt32>(subPassDescription.colorAttachments.size());
			vkSubPassDesc.pColorAttachments = vkColorAttachmentReferences[i].data();

			//DepthStencil attachment
			if (subPassDescription.depthStencilAttachment)
			{
				vkDepthStencilAttachment[i] = { subPassDescription.depthStencilAttachment->attachmentIndex, Converters::ToVulkan(subPassDescription.depthStencilAttachment->imageLayout) };
				vkSubPassDesc.pDepthStencilAttachment = &vkDepthStencilAttachment[i];
			}

			//Preserve attachment
			vkSubPassDesc.preserveAttachmentCount = static_cast<UInt32>(subPassDescription.preserveAttachments.size());
			vkSubPassDesc.pPreserveAttachments = subPassDescription.preserveAttachments.data();
			++i;
		}

		for (const auto& subPassDependency : subPassDependencies)
		{
			auto& vkSubPassDependency = vkSubPassDependencies.emplace_back();
			vkSubPassDependency.srcSubpass = subPassDependency.srcSubPassIndex;
			vkSubPassDependency.srcStageMask = Converters::ToVulkan<VkPipelineStageFlagBits>(subPassDependency.srcStageMask);
			vkSubPassDependency.srcAccessMask = Converters::ToVulkan<VkAccessFlagBits>(subPassDependency.srcAccessFlags);
			vkSubPassDependency.dstSubpass = subPassDependency.dstSubPassIndex;
			vkSubPassDependency.dstStageMask = Converters::ToVulkan<VkPipelineStageFlagBits>(subPassDependency.dstStageMask);
			vkSubPassDependency.dstAccessMask = Converters::ToVulkan<VkAccessFlagBits>(subPassDependency.dstAccessFlags);
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
		return std::make_unique<VkRHIMaterialBuilder>(*this, VkExtent2D{ windowExtent.X(), windowExtent.Y() });
	}

	std::unique_ptr<TextureBuilder> VkRHIDevice::CreateTextureBuilder()
	{
		auto& uploadContext = GetUploadContext();
		return std::make_unique<VkRHITextureBuilder>(*this, uploadContext, GetQueue(vk::Queue::Type::Graphics));
	}

	std::unique_ptr<CommandPool> VkRHIDevice::CreateCommandPool(rhi::QueueFamily family)
	{
		return std::make_unique<VkRHICommandPool>(*this, family);
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
		VkRHIMesh mesh(meshPath);
		return mesh.BuildGpuMesh(materialBuilder, renderPass, *this);
	}

	vk::UploadContext& VkRHIDevice::GetUploadContext()
	{
		if (m_uploadContext.has_value() == false)
			m_uploadContext.emplace(*this, GetQueueFamilyIndex(vk::Queue::Type::Graphics));
		return m_uploadContext.value();
	}

	vk::Instance& VkRHIDevice::GetVkInstance() const
	{
		return vk::Device::GetInstance();
	}

	void VkRHIDevice::WaitIdle()
	{
		vk::Device::WaitIdle();
	}

	std::unique_ptr<Sampler> VkRHIDevice::CreateSampler(SamplerFilter minFilter, SamplerFilter magFilter, SamplerAddressMode addressMode)
	{
		return std::make_unique<VkRHISampler>(*this, minFilter, magFilter, addressMode);
	}

	std::unique_ptr<Texture> VkRHIDevice::CreateTexture(PixelFormat format, Int32 width, Int32 height, TextureUsageFlags usage)
	{
		return std::make_unique<VkRHITexture>(*this, format, width, height, VK_IMAGE_ASPECT_COLOR_BIT, usage);
	}

	void VkRHIDevice::UpdateDescriptorSets(std::span<VkWriteDescriptorSet> writes)
	{
		vk::Device::UpdateDescriptorSetsWrite(writes);
	}
} //cct::Graphics::RHI
