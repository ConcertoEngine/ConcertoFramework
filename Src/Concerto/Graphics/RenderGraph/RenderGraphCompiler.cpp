//
// Created by arthur on 11/05/2026.
//

#include "Concerto/Graphics/RenderGraph/RenderGraphCompiler.hpp"

#include <algorithm>
#include <queue>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/RenderGraph/RenderGraphResourceRegistry.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"

namespace cct::gfx::rhi
{
	std::vector<RGCompiledPass> RenderGraphCompiler::Compile(
		std::vector<RGPass>& passes,
		RenderGraphResourceRegistry& registry,
		Device& device,
		RGTextureHandle finalOutput)
	{
		if (passes.empty())
			return {};

		BuildDependencies(passes);
		CullPasses(passes, finalOutput);
		const std::vector<UInt32> order = TopologicalSort(passes);

		std::vector<RGCompiledPass> compiled;
		compiled.reserve(order.size());

		std::unordered_set<UInt16> alreadyWritten;

		for (const UInt32 passIdx : order)
		{
			RGPass& pass = passes[passIdx];

			pass.colorAttachments.clear();
			pass.depthAttachment.reset();
			for (const auto& usage : pass.textureUsages)
			{
				if (usage.access == RGResourceAccess::Write)
				{
					if (usage.isDepth)
						pass.depthAttachment = usage.handle;
					else
						pass.colorAttachments.push_back(usage.handle);
				}
			}

			RGCompiledPass cp;
			cp.passIndex = passIdx;

			if (pass.type == RGPassType::Graphics)
				BuildRenderPassObjects(cp, pass, passIdx, alreadyWritten, registry, device);

			compiled.push_back(std::move(cp));

			for (const auto& usage : pass.textureUsages)
			{
				if (usage.access == RGResourceAccess::Write)
					alreadyWritten.insert(usage.handle.index);
			}
		}

		// Merge consecutive Graphics passes that target the same attachments into a single render pass.
		// Condition: same color+depth attachment set, and the next pass does not read from any attachment
		// the group writes (which would require an input attachment or a barrier inside the render pass).
		for (std::size_t i = 0; i < compiled.size(); ++i)
		{
			if (!compiled[i].renderPass)
				continue; // already absorbed into a previous group

			const RGPass& groupRoot = passes[compiled[i].passIndex];
			if (groupRoot.type != RGPassType::Graphics)
				continue;

			for (std::size_t j = i + 1; j < compiled.size(); ++j)
			{
				const RGPass& next = passes[compiled[j].passIndex];

				if (next.type != RGPassType::Graphics)
					break;
				if (next.colorAttachments != groupRoot.colorAttachments)
					break;
				if (next.depthAttachment != groupRoot.depthAttachment)
					break;

				bool conflict = false;
				for (const RGTextureHandle written : groupRoot.colorAttachments)
				{
					for (const auto& usage : next.textureUsages)
					{
						if (usage.handle.index == written.index && usage.access == RGResourceAccess::Read)
						{
							conflict = true;
							break;
						}
					}
					if (conflict)
						break;
				}
				if (!conflict && groupRoot.depthAttachment)
				{
					for (const auto& usage : next.textureUsages)
					{
						if (usage.handle.index == groupRoot.depthAttachment->index &&
							usage.access == RGResourceAccess::Read)
						{
							conflict = true;
							break;
						}
					}
				}
				if (conflict)
					break;

				// Merge: current pass does not close, next pass does not open its own render pass
				compiled[j - 1].closesRenderPass = false;
				compiled[j].renderPass = nullptr;
			}
		}

		return compiled;
	}

	void RenderGraphCompiler::BuildDependencies(std::vector<RGPass>& passes)
	{
		const UInt32 passCount = static_cast<UInt32>(passes.size());
		m_deps.assign(passCount, {});
		m_adjOut.assign(passCount, {});
		m_textureLastWriter.clear();
		m_bufferLastWriter.clear();

		auto addEdge = [&](UInt32 a, UInt32 b)
		{
			auto& inDeps = m_deps[b].incoming;
			if (std::find(inDeps.begin(), inDeps.end(), a) == inDeps.end())
			{
				inDeps.push_back(a);
				m_adjOut[a].push_back(b);
			}
		};

		for (UInt32 passIdx = 0; passIdx < passCount; ++passIdx)
		{
			for (const auto& usage : passes[passIdx].textureUsages)
			{
				if (usage.access == RGResourceAccess::Read)
				{
					auto it = m_textureLastWriter.find(usage.handle.index);
					if (it != m_textureLastWriter.end() && it->second != passIdx)
						addEdge(it->second, passIdx);
				}
				else
				{
					// WAW dependency: preserve submission order between consecutive writers of the
					// same resource.  Without this, a pass that has a RAW dep on another RTT can
					// be delayed past later write-only passes on this RTT, reversing paint order.
					auto prevIt = m_textureLastWriter.find(usage.handle.index);
					if (prevIt != m_textureLastWriter.end() && prevIt->second != passIdx)
						addEdge(prevIt->second, passIdx);
					m_textureLastWriter[usage.handle.index] = passIdx;
				}
			}
			for (const auto& usage : passes[passIdx].bufferUsages)
			{
				if (usage.access == RGResourceAccess::Read)
				{
					auto it = m_bufferLastWriter.find(usage.handle.index);
					if (it != m_bufferLastWriter.end() && it->second != passIdx)
						addEdge(it->second, passIdx);
				}
				else
				{
					auto prevIt = m_bufferLastWriter.find(usage.handle.index);
					if (prevIt != m_bufferLastWriter.end() && prevIt->second != passIdx)
						addEdge(prevIt->second, passIdx);
					m_bufferLastWriter[usage.handle.index] = passIdx;
				}
			}
		}
	}

	void RenderGraphCompiler::CullPasses(std::vector<RGPass>& passes, RGTextureHandle finalOutput)
	{
		const UInt32 passCount = static_cast<UInt32>(passes.size());
		std::vector<bool> alive(passCount, false);

		// BFS backwards: start from side-effect passes and the finalOutput writer
		std::vector<UInt32> workList;

		for (UInt32 i = 0; i < passCount; ++i)
		{
			if (passes[i].sideEffects)
				workList.push_back(i);
		}

		if (finalOutput.IsValid())
		{
			auto it = m_textureLastWriter.find(finalOutput.index);
			if (it != m_textureLastWriter.end())
				workList.push_back(it->second);
		}

		while (!workList.empty())
		{
			const UInt32 passIdx = workList.back();
			workList.pop_back();

			if (alive[passIdx])
				continue;
			alive[passIdx] = true;
			passes[passIdx].refCount++;

			for (const UInt32 dep : m_deps[passIdx].incoming)
			{
				if (!alive[dep])
					workList.push_back(dep);
			}
		}

		for (UInt32 i = 0; i < passCount; ++i)
			passes[i].culled = !alive[i];
	}

	std::vector<UInt32> RenderGraphCompiler::TopologicalSort(const std::vector<RGPass>& passes)
	{
		const UInt32 passCount = static_cast<UInt32>(passes.size());

		// Build in-degree only among non-culled passes
		std::vector<UInt32> inDeg(passCount, 0);
		for (UInt32 passIdx = 0; passIdx < passCount; ++passIdx)
		{
			if (passes[passIdx].culled)
				continue;
			for (const UInt32 dep : m_deps[passIdx].incoming)
			{
				if (!passes[dep].culled)
					++inDeg[passIdx];
			}
		}

		// Kahn's algorithm
		std::queue<UInt32> q;
		for (UInt32 i = 0; i < passCount; ++i)
		{
			if (!passes[i].culled && inDeg[i] == 0)
				q.push(i);
		}

		std::vector<UInt32> order;
		order.reserve(passCount);

		while (!q.empty())
		{
			const UInt32 curr = q.front();
			q.pop();
			order.push_back(curr);

			for (const UInt32 next : m_adjOut[curr])
			{
				if (passes[next].culled)
					continue;
				if (--inDeg[next] == 0)
					q.push(next);
			}
		}

		CCT_ASSERT(order.size() == static_cast<size_t>(std::count_if(passes.begin(), passes.end(),
																	 [](const RGPass& p)
																	 { return !p.culled; })),
				   "RenderGraphCompiler::TopologicalSort: cycle detected in render graph");

		return order;
	}

	void RenderGraphCompiler::BuildRenderPassObjects(RGCompiledPass& compiled, RGPass& pass,
													 UInt32 passIdx,
													 const std::unordered_set<UInt16>& alreadyWritten,
													 const RenderGraphResourceRegistry& registry,
													 Device& device)
	{
		std::vector<RenderPass::Attachment> attachments;
		std::vector<AttachmentReference> colorRefs;

		// Color attachments
		for (const RGTextureHandle handle : pass.colorAttachments)
		{
			const auto& desc = registry.GetDesc(handle);
			const bool firstWrite = (alreadyWritten.find(handle.index) == alreadyWritten.end());

			const AttachmentLoadOp loadOp = firstWrite
												? AttachmentLoadOp::Clear
												: AttachmentLoadOp::Load;

			attachments.push_back({
				desc.format,
				loadOp,
				AttachmentLoadOp::DontCare,
				AttachmentStoreOp::Store,
				AttachmentStoreOp::DontCare,
				ImageLayout::ColorAttachmentOptimal,
				ImageLayout::ColorAttachmentOptimal,
			});
			colorRefs.push_back({static_cast<UInt32>(colorRefs.size()), ImageLayout::ColorAttachmentOptimal});
		}

		// Depth attachment
		std::optional<AttachmentReference> depthRef;
		if (pass.depthAttachment.has_value())
		{
			const RGTextureHandle handle = *pass.depthAttachment;
			const auto& desc = registry.GetDesc(handle);
			const bool firstWrite = (alreadyWritten.find(handle.index) == alreadyWritten.end());

			const AttachmentLoadOp loadOp = firstWrite
												? AttachmentLoadOp::Clear
												: AttachmentLoadOp::Load;

			const UInt32 depthAttachIdx = static_cast<UInt32>(attachments.size());
			attachments.push_back({
				desc.format,
				loadOp,
				AttachmentLoadOp::DontCare,
				AttachmentStoreOp::Store,
				AttachmentStoreOp::DontCare,
				ImageLayout::DepthStencilAttachmentOptimal,
				ImageLayout::DepthStencilAttachmentOptimal,
			});
			depthRef = {depthAttachIdx, ImageLayout::DepthStencilAttachmentOptimal};
		}

		RenderPass::SubPassDescription subpass;
		subpass.colorAttachments = colorRefs;
		subpass.depthStencilAttachment = depthRef;

		const std::array deps = {
			RenderPass::SubPassDependency{
				RenderPass::SubPassDependency::ExternalSubPass,
				PipelineStage::ColorAttachmentOutput,
				MemoryAccessFlags{},
				0,
				PipelineStage::ColorAttachmentOutput,
				MemoryAccessFlags{MemoryAccess::ColorAttachmentWrite},
			},
			RenderPass::SubPassDependency{
				0,
				PipelineStage::ColorAttachmentOutput,
				MemoryAccess::ColorAttachmentWrite,
				RenderPass::SubPassDependency::ExternalSubPass,
				PipelineStage::FragmentShader,
				MemoryAccess::ShaderRead,
			},
		};

		std::vector<RenderPass::SubPassDescription> subpassVec{subpass};
		std::vector<RenderPass::SubPassDependency> depsVec{deps[0], deps[1]};

		compiled.renderPass = device.CreateRenderPass(
			std::span<RenderPass::Attachment>(attachments),
			std::span<RenderPass::SubPassDescription>(subpassVec),
			std::span<RenderPass::SubPassDependency>(depsVec));

		(void)passIdx;
	}

	ImageLayout RenderGraphCompiler::RequiredLayout(const RGTextureUsage& usage)
	{
		if (usage.access == RGResourceAccess::Read)
			return ImageLayout::ShaderReadOnlyOptimal;
		return usage.isDepth
				   ? ImageLayout::DepthStencilAttachmentOptimal
				   : ImageLayout::ColorAttachmentOptimal;
	}

	std::tuple<PipelineStageFlags, PipelineStageFlags, MemoryAccessFlags, MemoryAccessFlags>
	RenderGraphCompiler::InferBarrierParams(ImageLayout oldLayout, ImageLayout newLayout)
	{
		using PS = PipelineStage;
		using MA = MemoryAccess;

		const PipelineStageFlags pipe = PS::Pipe;
		const PipelineStageFlags caoStage = PS::ColorAttachmentOutput;
		const PipelineStageFlags fsStage = PS::FragmentShader;
		const PipelineStageFlags eftStage = PS::EarlyFragmentTests;
		const PipelineStageFlags lftStage = PS::LateFragmentTests;
		const PipelineStageFlags tfStage = PS::Transfer;
		const PipelineStageFlags botStage = PS::BottomOfPipe;

		const MemoryAccessFlags caWrite = MA::ColorAttachmentWrite;
		const MemoryAccessFlags caRead = MA::ColorAttachmentRead;
		const MemoryAccessFlags dsWrite = MA::DepthStencilAttachmentWrite;
		const MemoryAccessFlags dsRead = MA::DepthStencilAttachmentRead;
		const MemoryAccessFlags shRead = MA::ShaderRead;
		const MemoryAccessFlags tfWrite = MA::TransferWrite;
		const MemoryAccessFlags tfRead = MA::TransferRead;
		const MemoryAccessFlags memWrite = MA::MemoryWrite;
		const MemoryAccessFlags memRead = MA::MemoryRead;

		// VK_PIPELINE_STAGE_ALL_COMMANDS_BIT = 0x00010000 (outside the enum, used as fallback)
		const PipelineStageFlags allCmds = PipelineStageFlags::FromRaw(0x00010000u);

		if (oldLayout == ImageLayout::Undefined && newLayout == ImageLayout::ColorAttachmentOptimal)
			return {pipe, caoStage, {}, caWrite};

		if (oldLayout == ImageLayout::Undefined && newLayout == ImageLayout::DepthStencilAttachmentOptimal)
			return {pipe, eftStage, {}, dsWrite};

		if (oldLayout == ImageLayout::Undefined && newLayout == ImageLayout::ShaderReadOnlyOptimal)
			return {pipe, fsStage, {}, shRead};

		if (oldLayout == ImageLayout::Undefined && newLayout == ImageLayout::TransferDstOptimal)
			return {pipe, tfStage, {}, tfWrite};

		if (oldLayout == ImageLayout::ColorAttachmentOptimal && newLayout == ImageLayout::ShaderReadOnlyOptimal)
			return {caoStage, fsStage, caWrite, shRead};

		if (oldLayout == ImageLayout::DepthStencilAttachmentOptimal && newLayout == ImageLayout::ShaderReadOnlyOptimal)
			return {lftStage, fsStage, dsWrite, shRead};

		if (oldLayout == ImageLayout::ShaderReadOnlyOptimal && newLayout == ImageLayout::ColorAttachmentOptimal)
			return {fsStage, caoStage, shRead, caWrite};

		if (oldLayout == ImageLayout::ShaderReadOnlyOptimal && newLayout == ImageLayout::DepthStencilAttachmentOptimal)
			return {fsStage, eftStage, shRead, dsWrite};

		if (oldLayout == ImageLayout::ShaderReadOnlyOptimal && newLayout == ImageLayout::TransferSrcOptimal)
			return {fsStage, tfStage, shRead, tfRead};

		if (oldLayout == ImageLayout::TransferDstOptimal && newLayout == ImageLayout::ShaderReadOnlyOptimal)
			return {tfStage, fsStage, tfWrite, shRead};

		if (oldLayout == ImageLayout::TransferDstOptimal && newLayout == ImageLayout::ColorAttachmentOptimal)
			return {tfStage, caoStage, tfWrite, caWrite};

		if (oldLayout == ImageLayout::ColorAttachmentOptimal && newLayout == ImageLayout::PresentSrcKhr)
			return {caoStage, botStage, caWrite, {}};

		if (oldLayout == ImageLayout::PresentSrcKhr && newLayout == ImageLayout::ColorAttachmentOptimal)
			return {botStage, caoStage, {}, caWrite};

		// Fallback: conservative all-commands barrier
		return {allCmds, allCmds, memWrite, memRead};
	}

	std::pair<PipelineStageFlags, MemoryAccessFlags>
	RenderGraphCompiler::BufferAccessParams(RGPassType type, RGResourceAccess access)
	{
		const bool isWrite = (access == RGResourceAccess::Write);

		switch (type)
		{
			case RGPassType::Compute:
				return {PipelineStageFlags{PipelineStage::ComputeShader},
						MemoryAccessFlags{isWrite ? MemoryAccess::ShaderWrite : MemoryAccess::ShaderRead}};

			case RGPassType::Transfer:
				return {PipelineStageFlags{PipelineStage::Transfer},
						MemoryAccessFlags{isWrite ? MemoryAccess::TransferWrite : MemoryAccess::TransferRead}};

			case RGPassType::Graphics:
				break;
		}

		const PipelineStageFlags stages = PipelineStageFlags{PipelineStage::VertexInput} |
										  PipelineStage::VertexShader |
										  PipelineStage::FragmentShader;

		if (isWrite)
			return {stages, MemoryAccessFlags{MemoryAccess::ShaderWrite}};

		return {stages, MemoryAccessFlags{MemoryAccess::ShaderRead} |
							MemoryAccess::UniformRead |
							MemoryAccess::VertexAttributeRead |
							MemoryAccess::IndexRead};
	}
} // namespace cct::gfx::rhi
