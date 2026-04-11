//
// Created by arthur on 14/06/22.
//

#ifndef CONCERTO_GRAPHICS_COMMANDBUFFER_HPP
#define CONCERTO_GRAPHICS_COMMANDBUFFER_HPP

#include <functional>
#include <span>
#include <memory>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;
	class Fence;
	class Pipeline;
	class PipelineLayout;
	class CommandPool;
	class Queue;
	class Buffer;
	class MeshPushConstants;
	class DescriptorSet;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API CommandBuffer : public Object<VkCommandBuffer>
	{
	public:
		CommandBuffer();
		CommandBuffer(CommandPool& owner, VkCommandBufferLevel level);
		~CommandBuffer() override;

		CommandBuffer(CommandBuffer&&) noexcept;
		CommandBuffer(const CommandBuffer&) = delete;

		CommandBuffer& operator=(CommandBuffer&&) noexcept;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		VkResult Create(CommandPool& owner, VkCommandBufferLevel level);

		void Reset() const;
		void Begin() const;
		void End() const;

		void BeginRenderPass(const VkRenderPassBeginInfo& info) const;
		void EndRenderPass() const;

		void BindPipeline(VkPipelineBindPoint pipelineBindPoint, const Pipeline& pipeline) const;
		void BindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) const;

		void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout pipelineLayout,UInt32 firstSet, UInt32 descriptorSetCount, const DescriptorSet& descriptorSet,UInt32 dynamicOffsets) const;
		void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout pipelineLayout,UInt32 firstSet, UInt32 descriptorSetCount, const DescriptorSet& descriptorSet) const;
		void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout pipelineLayout, std::span<VkDescriptorSet> descriptorSets) const;
		void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout pipelineLayout, std::span<DescriptorSet> descriptorSets) const;
		void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout pipelineLayout, std::span<std::shared_ptr<DescriptorSet>> descriptorSets) const;

		void BindVertexBuffers(const Buffer& buffer) const;

		void UpdatePushConstants(const PipelineLayout& pipelineLayout, const MeshPushConstants& meshPushConstants) const;
		void UpdatePushConstants(VkPipelineLayout pipelineLayout, const MeshPushConstants& meshPushConstants) const;

		void Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex,
				UInt32 firstInstance) const;
		void DrawIndirect(const Buffer& buffer, UInt32 offset, UInt32 drawCount, UInt32 stride) const;

		
		void ImmediateSubmit(const Fence& fence, const CommandPool& commandPool, const Queue& queue,
				std::function<void(CommandBuffer&)>&& function);
		void Submit(const Fence& fence, const CommandPool& commandPool, const Queue& queue);

		void ExecuteCommands(std::span<CommandBuffer> commandBuffers) const;

		void CopyBuffer(const Buffer& src, const Buffer& dest, std::size_t size, std::size_t srcOffset = 0, std::size_t dstOffset = 0) const;

		void SetViewport(const VkViewport& viewport) const;
		void SetScissor(VkRect2D scissor) const;

	private:
		vk::CommandPool* m_commandPool;
		VkCommandBufferLevel m_level;
	};
} // namespace cct::gfx::vk

#endif //CONCERTO_GRAPHICS_COMMANDBUFFER_HPP
