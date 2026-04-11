//
// Created by arthur on 24/03/2022.
//

//#include <vulkan/utility/vk_format_utils.h>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"

namespace VulkanInitializer
{
	VkPipelineShaderStageCreateInfo
	PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule)
	{
		VkPipelineShaderStageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.pNext = nullptr;
		info.stage = stage;
		info.module = shaderModule;
		info.pName = "main";
		return info;
	}

	VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo()
	{
		VkPipelineVertexInputStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		info.pNext = nullptr;
		info.vertexBindingDescriptionCount = 0;
		info.vertexAttributeDescriptionCount = 0;
		return info;
	}

	VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo(VkPrimitiveTopology topology)
	{
		VkPipelineInputAssemblyStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		info.pNext = nullptr;

		info.topology = topology;
		info.primitiveRestartEnable = VK_FALSE;
		return info;
	}

	VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo(VkPolygonMode polygonMode)
	{
		VkPipelineRasterizationStateCreateInfo info = {};

		info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		info.pNext = nullptr;
		info.depthClampEnable = VK_FALSE;
		info.rasterizerDiscardEnable = VK_FALSE;
		info.polygonMode = polygonMode;
		info.lineWidth = 1.0f;
		info.cullMode = VK_CULL_MODE_NONE;
		info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		info.depthBiasEnable = VK_FALSE;
		info.depthBiasConstantFactor = 0.0f;
		info.depthBiasClamp = 0.0f;
		info.depthBiasSlopeFactor = 0.0f;

		return info;
	}

	VkPipelineMultisampleStateCreateInfo MultisamplingStateCreateInfo()
	{
		VkPipelineMultisampleStateCreateInfo info = {};

		info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		info.pNext = nullptr;
		info.sampleShadingEnable = VK_FALSE;
		//multisampling defaulted to no multisampling (1 sample per pixel)
		info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		info.minSampleShading = 1.0f;
		info.pSampleMask = nullptr;
		info.alphaToCoverageEnable = VK_FALSE;
		info.alphaToOneEnable = VK_FALSE;

		return info;
	}

	VkPipelineColorBlendAttachmentState ColorBlendAttachmentState()
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
											  VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		return colorBlendAttachment;
	}

	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo()
	{
		VkPipelineLayoutCreateInfo info = {};

		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.setLayoutCount = 0;
		info.pSetLayouts = nullptr;
		info.pushConstantRangeCount = 0;
		info.pPushConstantRanges = nullptr;

		return info;
	}

	VkRenderPassBeginInfo
	RenderPassBeginInfo(VkRenderPass renderPass, VkExtent2D windowExtent, VkFramebuffer framebuffer)
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.pNext = nullptr;

		info.renderPass = renderPass;
		info.renderArea.offset.x = 0;
		info.renderArea.offset.y = 0;
		info.renderArea.extent = windowExtent;
		info.clearValueCount = 1;
		info.pClearValues = nullptr;
		info.framebuffer = framebuffer;

		return info;
	}

	VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
	{
		VkImageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.pNext = nullptr;

		info.imageType = VK_IMAGE_TYPE_2D;

		info.format = format;
		info.extent = extent;

		info.mipLevels = 1; //vkuFormatComponentCount(format);
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = usageFlags;

		return info;
	}

	VkImageViewCreateInfo
	ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
	{
		//build a image-view for the depth image to use for rendering
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.pNext = nullptr;

		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.image = image;
		info.format = format;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;
		info.subresourceRange.aspectMask = aspectFlags;

		return info;
	}

	VkPipelineDepthStencilStateCreateInfo
	DepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp)
	{
		VkPipelineDepthStencilStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		info.pNext = nullptr;

		info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
		info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
		info.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
		info.depthBoundsTestEnable = VK_FALSE;
		info.minDepthBounds = 0.0f; // Optional
		info.maxDepthBounds = 1.0f; // Optional
		info.stencilTestEnable = VK_FALSE;

		return info;
	}

	VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlagBits bits)
	{
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.pNext = nullptr;

		info.pInheritanceInfo = nullptr;
		info.flags = bits;
		return info;
	}

	VkSubmitInfo SubmitInfo(VkCommandBuffer* cmd)
	{
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = nullptr;

		info.waitSemaphoreCount = 0;
		info.pWaitSemaphores = nullptr;
		info.pWaitDstStageMask = nullptr;
		info.commandBufferCount = 1;
		info.pCommandBuffers = cmd;
		info.signalSemaphoreCount = 0;
		info.pSignalSemaphores = nullptr;

		return info;
	}

	VkPresentInfoKHR PresentInfo()
	{
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.pNext = nullptr;

		info.swapchainCount = 0;
		info.pSwapchains = nullptr;
		info.pWaitSemaphores = nullptr;
		info.waitSemaphoreCount = 0;
		info.pImageIndices = nullptr;

		return info;
	}


	VkCommandPoolCreateInfo
	CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
	{
		VkCommandPoolCreateInfo info = {};

		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;
		info.queueFamilyIndex = queueFamilyIndex;
		info.flags = flags;

		return info;
	}

	VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlagBits bits)
	{
		VkFenceCreateInfo info = {};

		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = bits;

		return info;
	}

	VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
	{
		VkSemaphoreCreateInfo info = {};

		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = flags;

		return info;
	}

	VkCommandBufferAllocateInfo
	CommandBufferAllocateInfo(VkCommandPool pool, int count, VkCommandBufferLevel level)
	{
		VkCommandBufferAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.pNext = nullptr;

		info.commandPool = pool;
		info.commandBufferCount = count;
		info.level = level;
		return info;
	}

	VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderPass, VkExtent2D extent)
	{
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.pNext = nullptr;

		info.renderPass = renderPass;
		info.attachmentCount = 1;
		info.width = extent.width;
		info.height = extent.height;
		info.layers = 1;

		return info;
	}

	VkDescriptorSetLayoutBinding
	DescriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
	{
		VkDescriptorSetLayoutBinding setbind = {};
		setbind.binding = binding;
		setbind.descriptorCount = 1;
		setbind.descriptorType = type;
		setbind.pImmutableSamplers = nullptr;
		setbind.stageFlags = stageFlags;

		return setbind;
	}

	VkWriteDescriptorSet WriteDescriptorBuffer(VkDescriptorType type, VkDescriptorSet dstSet,
			VkDescriptorBufferInfo* bufferInfo, uint32_t binding)
	{
		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;

		write.dstBinding = binding;
		write.dstSet = dstSet;
		write.descriptorCount = 1;
		write.descriptorType = type;
		write.pBufferInfo = bufferInfo;

		return write;
	}

	VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
	{
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.pNext = nullptr;

		info.pInheritanceInfo = nullptr;
		info.flags = flags;
		return info;
	}

	VkSamplerCreateInfo SamplerCreateInfo(VkFilter filter, VkSamplerAddressMode samplerAddressMode)
	{

		VkSamplerCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		info.pNext = nullptr;

		info.magFilter = filter;
		info.minFilter = filter;
		info.addressModeU = samplerAddressMode;
		info.addressModeV = samplerAddressMode;
		info.addressModeW = samplerAddressMode;

		return info;

	}

	VkWriteDescriptorSet
	WriteDescriptorImage(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, cct::UInt32 binding)
	{
		{
			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;

			write.dstBinding = binding;
			write.dstSet = dstSet;
			write.descriptorCount = 1;
			write.descriptorType = type;
			write.pImageInfo = imageInfo;

			return write;
		}
	}
}