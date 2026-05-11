//
// Created by arthur on 14/06/22.
//

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer/CommandBuffer.hpp"

#include <utility>

#include "Concerto/Graphics/Backend/Vulkan/Utils.hpp"
#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Buffer/Buffer.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandPool/CommandPool.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorSet/DescriptorSet.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Fence/Fence.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Pipeline/Pipeline.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PipelineLayout/PipelineLayout.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Queue/Queue.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"

namespace cct::gfx::vk
{
	CommandBuffer::CommandBuffer() :
		m_commandPool(nullptr),
		m_level()
	{
	}

	CommandBuffer::CommandBuffer(CommandPool& owner, VkCommandBufferLevel level) :
		Object(*owner.GetDevice()),
		m_commandPool(&owner),
		m_level(level)
	{
		if (Create(owner, level) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	CommandBuffer::~CommandBuffer()
	{
		if (m_handle == VK_NULL_HANDLE)
			return;
		if (m_device)
		{
			m_device->WaitIdle();
			m_device->vkFreeCommandBuffers(*m_device->Get(), *m_commandPool->Get(), 1, &m_handle);
		}
	}

	CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept :
		Object(std::move(other)),
		m_commandPool(std::exchange(other.m_commandPool, VK_NULL_HANDLE)),
		m_level(std::exchange(other.m_level, VK_COMMAND_BUFFER_LEVEL_MAX_ENUM))
	{
	}

	CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept
	{
		std::swap(m_device, other.m_device);
		std::swap(m_commandPool, other.m_commandPool);
		std::swap(m_handle, other.m_handle);

		Object::operator=(std::move(other));
		return *this;
	}

	VkResult CommandBuffer::Create(CommandPool& owner, VkCommandBufferLevel level)
	{
		m_device = owner.GetDevice();
		m_commandPool = &owner;
		m_level = level;

		VkCommandBufferAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.pNext = nullptr;
		info.commandPool = *owner.Get();
		info.commandBufferCount = 1;
		info.level = level;

		m_lastResult = m_device->vkAllocateCommandBuffers(*m_device->Get(), &info, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkAllocateCommandBuffers failed VkResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}

	void CommandBuffer::Reset() const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		const VkResult result = m_device->vkResetCommandBuffer(m_handle, 0);
		CCT_ASSERT(result == VK_SUCCESS, "ConcertoGraphics: vkResetCommandBuffer VKResult={}", static_cast<int>(result));
	}

	void CommandBuffer::Begin() const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		VkCommandBufferBeginInfo cmdBeginInfo = {};

		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.pNext = nullptr;
		cmdBeginInfo.pInheritanceInfo = nullptr;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo;
		if (m_level == VK_COMMAND_BUFFER_LEVEL_SECONDARY)
		{
			commandBufferInheritanceInfo = {};
			commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			cmdBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;
		}

		const VkResult result = m_device->vkBeginCommandBuffer(m_handle, &cmdBeginInfo);
		CCT_ASSERT(result == VK_SUCCESS, "ConcertoGraphics: vkBeginCommandBuffer failed VKResult={}", static_cast<int>(result));
#ifdef CCT_ENABLE_OBJECT_DEBUG
		if (m_device->IsExtensionEnabled(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
		{
			VkDebugMarkerMarkerInfoEXT markerInfo = {
				.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
				.pNext = nullptr,
				.pMarkerName = ObjectDebug::GetDebugName().data(),
				.color = {1.f, 1.f, 0.f}};
			m_device->vkCmdDebugMarkerBeginEXT(m_handle, &markerInfo);
		}
#endif
	}

	void CommandBuffer::End() const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

#ifdef CCT_ENABLE_OBJECT_DEBUG
		if (m_device->IsExtensionEnabled(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
		{
			m_device->vkCmdDebugMarkerEndEXT(m_handle);
		}
#endif

		const VkResult result = m_device->vkEndCommandBuffer(m_handle);
		CCT_ASSERT(result == VK_SUCCESS, "ConcertoGraphics: vkEndCommandBuffer failed VKResult={}", static_cast<int>(result));
	}

	void CommandBuffer::BeginRenderPass(const VkRenderPassBeginInfo& info) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdBeginRenderPass(m_handle, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	void CommandBuffer::EndRenderPass() const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdEndRenderPass(m_handle);
	}

	void CommandBuffer::BindPipeline(const VkPipelineBindPoint pipelineBindPoint, const Pipeline& pipeline) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdBindPipeline(m_handle, pipelineBindPoint, *pipeline.Get());
	}

	void CommandBuffer::BindPipeline(const VkPipelineBindPoint pipelineBindPoint, const VkPipeline pipeline) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdBindPipeline(m_handle, pipelineBindPoint, pipeline);
	}

	void CommandBuffer::Draw(const UInt32 vertexCount, const UInt32 instanceCount, const UInt32 firstVertex, const UInt32 firstInstance) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void CommandBuffer::DrawIndirect(const Buffer& buffer, const UInt32 offset, const UInt32 drawCount, const UInt32 stride) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdDrawIndirect(m_handle, *buffer.Get(), offset, drawCount, stride);
	}

	void CommandBuffer::BindVertexBuffers(const Buffer& buffer) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		VkDeviceSize offset = 0;
		m_device->vkCmdBindVertexBuffers(m_handle, 0, 1, buffer.Get(), &offset);
	}

	void CommandBuffer::UpdatePushConstants(const PipelineLayout& pipelineLayout, const MeshPushConstants& meshPushConstants) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdPushConstants(m_handle, *pipelineLayout.Get(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &meshPushConstants);
	}

	void CommandBuffer::UpdatePushConstants(const VkPipelineLayout pipelineLayout, const MeshPushConstants& meshPushConstants) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdPushConstants(m_handle, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &meshPushConstants);
	}

	void CommandBuffer::BindDescriptorSets(const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout pipelineLayout, const UInt32 firstSet, const UInt32 descriptorSetCount, const DescriptorSet& descriptorSet, const UInt32 dynamicOffsets) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdBindDescriptorSets(m_handle, pipelineBindPoint, pipelineLayout, firstSet, descriptorSetCount, descriptorSet.Get(), 1, &dynamicOffsets);
	}

	void CommandBuffer::BindDescriptorSets(const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout pipelineLayout, const UInt32 firstSet, const UInt32 descriptorSetCount, const DescriptorSet& descriptorSet) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdBindDescriptorSets(m_handle, pipelineBindPoint, pipelineLayout, firstSet, descriptorSetCount, descriptorSet.Get(), 0, nullptr);
	}

	void CommandBuffer::BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout pipelineLayout, std::span<VkDescriptorSet> descriptorSets) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		m_device->vkCmdBindDescriptorSets(m_handle, pipelineBindPoint, pipelineLayout, 0, static_cast<UInt32>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
	}

	void CommandBuffer::BindDescriptorSets(const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout pipelineLayout, const std::span<DescriptorSet> descriptorSets) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		std::vector<VkDescriptorSet> vkDescriptorSets;
		vkDescriptorSets.reserve(descriptorSets.size());
		for (const auto& descriptorSet : descriptorSets)
			vkDescriptorSets.push_back(*descriptorSet.Get());
		m_device->vkCmdBindDescriptorSets(m_handle, pipelineBindPoint, pipelineLayout, 0, static_cast<UInt32>(vkDescriptorSets.size()), vkDescriptorSets.data(), 0, nullptr);
	}

	void CommandBuffer::BindDescriptorSets(const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout pipelineLayout, const std::span<DescriptorSetPtr> descriptorSets) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		std::vector<VkDescriptorSet> vkDescriptorSets;
		vkDescriptorSets.reserve(descriptorSets.size());
		for (const auto& descriptorSet : descriptorSets)
			vkDescriptorSets.push_back(*descriptorSet->Get());
		m_device->vkCmdBindDescriptorSets(m_handle, pipelineBindPoint, pipelineLayout, 0, static_cast<UInt32>(vkDescriptorSets.size()), vkDescriptorSets.data(), 0, nullptr);
	}

	void CommandBuffer::ImmediateSubmit(const Fence& fence, const CommandPool& commandPool, const Queue& queue, std::function<void(CommandBuffer&)>&& function)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		Begin();
		{
			function(*this);
		}
		End();

		Submit(fence, commandPool, queue);
	}

	void CommandBuffer::Submit(const Fence& fence, const CommandPool& commandPool, const Queue& queue)
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		const VkSubmitInfo submitInfo = VulkanInitializer::SubmitInfo(&m_handle);
		const VkResult result = m_device->vkQueueSubmit(*queue.Get(), 1, &submitInfo, *fence.Get());
		CCT_ASSERT(result == VK_SUCCESS, "ConcertoGraphics: vkQueueSubmit failed VKResult={}", static_cast<int>(result));

		fence.Wait(9999999999);
		fence.Reset();
		commandPool.Reset();
	}

	void CommandBuffer::ExecuteCommands(std::span<CommandBuffer> commandBuffers) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");
		CCT_PROFILER_SCOPE();

		std::vector<VkCommandBuffer> vkCommandBuffers;
		vkCommandBuffers.reserve(commandBuffers.size());

		for (auto& cb : commandBuffers)
			vkCommandBuffers.push_back(*cb.Get());

		m_device->vkCmdExecuteCommands(*Get(), vkCommandBuffers.size(), vkCommandBuffers.data());
	}

	void CommandBuffer::CopyBuffer(const Buffer& src, const Buffer& dest, const std::size_t size, std::size_t srcOffset, std::size_t dstOffset) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		const VkBufferCopy copyRegion = {
			.srcOffset = srcOffset,
			.dstOffset = dstOffset,
			.size = size};
		m_device->vkCmdCopyBuffer(m_handle, *src.Get(), *dest.Get(), 1, &copyRegion);
	}

	void CommandBuffer::SetViewport(const VkViewport& viewport) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		m_device->vkCmdSetViewport(m_handle, 0, 1, &viewport);
	}

	void CommandBuffer::SetScissor(const VkRect2D scissor) const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		m_device->vkCmdSetScissor(m_handle, 0, 1, &scissor);
	}
} // namespace cct::gfx::vk
