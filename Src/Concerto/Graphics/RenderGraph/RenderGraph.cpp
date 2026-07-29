//
// Created by arthur on 11/05/2026.
//

#include "Concerto/Graphics/RenderGraph/RenderGraph.hpp"

#include <ostream>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/RenderGraph/RenderGraphCompiler.hpp"

namespace cct::gfx::rhi
{
	RenderGraph::RenderGraph(Device& device, UInt32 maxFramesInFlight) :
		m_device(device),
		m_maxFramesInFlight(maxFramesInFlight)
	{
	}

	RGTextureHandle RenderGraph::CreateTexture(const RGTextureDesc& desc)
	{
		MarkDirty();
		return m_registry.RegisterTexture(desc);
	}

	RGBufferHandle RenderGraph::CreateBuffer(const RGBufferDesc& desc)
	{
		MarkDirty();
		return m_registry.RegisterBuffer(desc);
	}

	RGTextureHandle RenderGraph::ImportTexture(const char* name,
											   std::shared_ptr<Texture> texture,
											   ImageLayout currentLayout,
											   PixelFormat format,
											   UInt32 width,
											   UInt32 height)
	{
		MarkDirty();
		return m_registry.ImportTexture(name, std::move(texture), currentLayout, format, width, height);
	}

	RGBufferHandle RenderGraph::ImportBuffer(const char* name,
											 std::shared_ptr<Buffer> buffer)
	{
		MarkDirty();
		return m_registry.ImportBuffer(name, std::move(buffer));
	}

	void RenderGraph::AddPass(const char* name, RGPassType type,
							  std::function<void(RenderGraphBuilder&)> setup,
							  std::function<void(RenderGraphContext&)> execute)
	{
		MarkDirty();

		RGPass pass;
		pass.name = name;
		pass.type = type;
		pass.execute = std::move(execute);

		RenderGraphBuilder builder(pass);
		setup(builder);

		m_passes.push_back(std::move(pass));
	}

	void RenderGraph::AddGraphicsPass(const char* name,
									  std::function<void(RenderGraphBuilder&)> setup,
									  std::function<void(RenderGraphContext&)> execute)
	{
		AddPass(name, RGPassType::Graphics, std::move(setup), std::move(execute));
	}

	void RenderGraph::AddComputePass(const char* name,
									 std::function<void(RenderGraphBuilder&)> setup,
									 std::function<void(RenderGraphContext&)> execute)
	{
		AddPass(name, RGPassType::Compute, std::move(setup), std::move(execute));
	}

	void RenderGraph::AddTransferPass(const char* name,
									  std::function<void(RenderGraphBuilder&)> setup,
									  std::function<void(RenderGraphContext&)> execute)
	{
		AddPass(name, RGPassType::Transfer, std::move(setup), std::move(execute));
	}

	void RenderGraph::SetFinalOutput(RGTextureHandle handle)
	{
		m_finalOutput = handle;
		MarkDirty();
	}

	void RenderGraph::Compile()
	{
		RetireCompiledPasses();
		m_compiledPasses = m_compiler.Compile(m_passes, m_registry, m_device, m_finalOutput);
		m_dirty = false;
	}

	void RenderGraph::AllocateResources()
	{
		if (m_dirty)
			Compile();

		m_registry.Allocate(m_device);
	}

	Texture& RenderGraph::GetTexture(RGTextureHandle handle)
	{
		return m_registry.GetTexture(handle);
	}

	void RenderGraph::RetireCompiledPasses()
	{
		for (RGCompiledPass& cp : m_compiledPasses)
		{
			if (cp.frameBuffer)
				m_retired.frameBuffers.push_back(std::move(cp.frameBuffer));
			for (auto& view : cp.attachmentViews)
				m_retired.attachmentViews.push_back(std::move(view));
			cp.attachmentViews.clear();
			if (cp.renderPass)
				m_retired.renderPasses.push_back(std::move(cp.renderPass));
		}
	}

	void RenderGraph::Execute(CommandBuffer& cmd, UInt32 frameWidth, UInt32 frameHeight)
	{
		AllocateResources();

		RenderPass* activeRenderPass = nullptr;

		struct BufferAccessState
		{
			PipelineStageFlags stage;
			MemoryAccessFlags access;
			bool isWrite;
		};
		std::unordered_map<UInt16, BufferAccessState> bufferStates;

		// Emit layout-transition barriers for all texture usages in a pass.
		// Must only be called when no render pass is active.
		const auto emitBarriersForPass = [&](const RGPass& p)
		{
			for (const RGTextureUsage& usage : p.textureUsages)
			{
				const ImageLayout required = RenderGraphCompiler::RequiredLayout(usage);
				const ImageLayout current = m_registry.GetCurrentLayout(usage.handle);
				if (current == required)
					continue;
				auto [srcStage, dstStage, srcAccess, dstAccess] =
					RenderGraphCompiler::InferBarrierParams(current, required);
				Texture& tex = m_registry.GetTexture(usage.handle);
				cmd.PipelineBarrier(tex, current, required, srcStage, dstStage, srcAccess, dstAccess);
				m_registry.SetCurrentLayout(usage.handle, required);
			}

			for (const RGBufferUsage& usage : p.bufferUsages)
			{
				auto [stage, access] = RenderGraphCompiler::BufferAccessParams(p.type, usage.access);
				const bool isWrite = (usage.access == RGResourceAccess::Write);

				const auto it = bufferStates.find(usage.handle.index);
				if (it == bufferStates.end())
				{
					bufferStates.emplace(usage.handle.index, BufferAccessState{stage, access, isWrite});
					continue;
				}

				BufferAccessState& prev = it->second;

				if (!prev.isWrite && !isWrite)
				{
					prev.stage |= stage;
					prev.access |= access;
					continue;
				}

				cmd.PipelineBarrier(m_registry.GetBuffer(usage.handle),
									prev.stage, stage, prev.access, access);
				prev = BufferAccessState{stage, access, isWrite};
			}
		};

		for (std::size_t i = 0; i < m_compiledPasses.size(); ++i)
		{
			RGCompiledPass& compiled = m_compiledPasses[i];
			const RGPass& pass = m_passes[compiled.passIndex];

			const bool isMergedGraphics = (pass.type == RGPassType::Graphics && !compiled.renderPass && activeRenderPass != nullptr);

			if (!isMergedGraphics)
				emitBarriersForPass(pass);

			if (pass.type == RGPassType::Graphics && compiled.renderPass)
			{
				for (std::size_t j = i + 1; j < m_compiledPasses.size(); ++j)
				{
					if (m_compiledPasses[j].renderPass)
						break; // next independent render-pass group
					const RGPass& mp = m_passes[m_compiledPasses[j].passIndex];
					if (mp.type != RGPassType::Graphics)
						break;
					emitBarriersForPass(mp);
				}

				if (!compiled.frameBuffer)
				{
					std::vector<std::unique_ptr<TextureView>> views;
					views.reserve(pass.colorAttachments.size() + (pass.depthAttachment.has_value() ? 1 : 0));

					for (const RGTextureHandle colorHandle : pass.colorAttachments)
						views.push_back(m_registry.GetTexture(colorHandle).CreateView());

					if (pass.depthAttachment.has_value())
						views.push_back(m_registry.GetTexture(*pass.depthAttachment).CreateView());

					UInt32 fbWidth = frameWidth, fbHeight = frameHeight;
					if (!pass.colorAttachments.empty())
					{
						const auto& desc = m_registry.GetDesc(pass.colorAttachments[0]);
						if (desc.width > 0 && desc.height > 0)
							fbWidth = desc.width, fbHeight = desc.height;
					}
					else if (pass.depthAttachment.has_value())
					{
						const auto& desc = m_registry.GetDesc(*pass.depthAttachment);
						if (desc.width > 0 && desc.height > 0)
							fbWidth = desc.width, fbHeight = desc.height;
					}

					compiled.frameBuffer = m_device.CreateFrameBuffer(fbWidth, fbHeight, *compiled.renderPass, views);
					compiled.attachmentViews = std::move(views);
				}

				m_currentFrameBuffer = compiled.frameBuffer.get();
				activeRenderPass = compiled.renderPass.get();
				cmd.BeginRenderPass(*compiled.renderPass, *m_currentFrameBuffer, Vector3f{0.f, 0.f, 0.f});
			}

			RenderGraphContext ctx(cmd, m_currentFrameBuffer, activeRenderPass, m_registry, frameWidth, frameHeight);
			cmd.BeginDebugLabel(pass.name.c_str());
			pass.execute(ctx);
			cmd.EndDebugLabel();

			if (pass.type == RGPassType::Graphics && compiled.closesRenderPass && m_currentFrameBuffer)
			{
				cmd.EndRenderPass();
				m_currentFrameBuffer = nullptr;
				activeRenderPass = nullptr;
			}
		}

		for (const auto& [_, pair] : m_registry.ExportLayouts())
		{
			const auto& [handle, exportLayout] = pair;
			const ImageLayout current = m_registry.GetCurrentLayout(handle);
			if (current == exportLayout)
				continue;
			auto [srcStage, dstStage, srcAccess, dstAccess] =
				RenderGraphCompiler::InferBarrierParams(current, exportLayout);
			Texture& tex = m_registry.GetTexture(handle);
			cmd.PipelineBarrier(tex, current, exportLayout, srcStage, dstStage, srcAccess, dstAccess);
			m_registry.SetCurrentLayout(handle, exportLayout);
		}
	}

	void RenderGraph::Reset()
	{
		m_pendingFrames.push_back(std::move(m_retired));
		m_retired = {};

		while (m_pendingFrames.size() > m_maxFramesInFlight)
		{
			auto& oldest = m_pendingFrames.front();
			m_registry.ReturnTransientsToPool(oldest.transientTextures, oldest.transientBuffers);
			m_pendingFrames.pop_front();
		}

		m_registry.Reset();
	}

	void RenderGraph::SetTextureLayout(RGTextureHandle handle, ImageLayout layout)
	{
		m_registry.SetCurrentLayout(handle, layout);
	}

	void RenderGraph::SetExportLayout(RGTextureHandle handle, ImageLayout layout)
	{
		m_registry.SetExportLayout(handle, layout);
	}

	ImageLayout RenderGraph::GetCurrentLayout(RGTextureHandle handle) const
	{
		return m_registry.GetCurrentLayout(handle);
	}

	void RenderGraph::MarkDirty()
	{
		m_dirty = true;
	}

	void RenderGraph::Clear()
	{
		m_registry.ExtractTransients(m_retired.transientTextures, m_retired.transientBuffers);
		RetireCompiledPasses();
		m_passes.clear();
		m_compiledPasses.clear();
		m_registry.ClearEntries();
		m_finalOutput = {};
		m_dirty = true;
	}

	void RenderGraph::DumpGraph(std::ostream& out) const
	{
		out << "RenderGraph (" << m_passes.size() << " declared, "
			<< m_compiledPasses.size() << " compiled)\n";

		for (const RGCompiledPass& cp : m_compiledPasses)
		{
			const RGPass& pass = m_passes[cp.passIndex];
			const char* typeStr =
				pass.type == RGPassType::Graphics ? "Graphics" : pass.type == RGPassType::Compute ? "Compute"
																								  : "Transfer";

			out << "  [" << typeStr << "] " << pass.name << "\n";

			for (const RGTextureUsage& usage : pass.textureUsages)
			{
				out << "    Texture[" << usage.handle.index << "] "
					<< (usage.access == RGResourceAccess::Read ? "READ" : "WRITE")
					<< (usage.isDepth ? " (depth)" : "") << "\n";
			}

			for (const RGBufferUsage& usage : pass.bufferUsages)
			{
				out << "    Buffer[" << usage.handle.index << "] "
					<< (usage.access == RGResourceAccess::Read ? "READ" : "WRITE") << "\n";
			}
		}

		for (UInt32 i = 0; i < static_cast<UInt32>(m_passes.size()); ++i)
		{
			if (m_passes[i].culled)
				out << "  [CULLED] " << m_passes[i].name << "\n";
		}
	}
} // namespace cct::gfx::rhi
