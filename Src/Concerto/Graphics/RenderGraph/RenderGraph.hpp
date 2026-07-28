//
// Created by arthur on 11/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_RENDERGRAPH_HPP
#define CONCERTO_GRAPHICS_RHI_RENDERGRAPH_HPP

#include <deque>
#include <functional>
#include <iosfwd>
#include <memory>
#include <vector>

#include "Concerto/Graphics/RenderGraph/RenderGraphBuilder.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphCompiler.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphContext.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphPass.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphResource.hpp"
#include "Concerto/Graphics/RenderGraph/RenderGraphResourceRegistry.hpp"
#include "Concerto/Graphics/RHI/CommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"
#include "Concerto/Graphics/RHI/FrameBuffer.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API RenderGraph
	{
	public:
		explicit RenderGraph(Device& device, UInt32 maxFramesInFlight = 3);
		RenderGraph(const RenderGraph&) = delete;
		RenderGraph& operator=(const RenderGraph&) = delete;
		RenderGraph(RenderGraph&&) = default;
		RenderGraph& operator=(RenderGraph&&) = delete;

		// Resource declaration
		RGTextureHandle CreateTexture(const RGTextureDesc& desc);
		RGBufferHandle CreateBuffer(const RGBufferDesc& desc);

		// Import external resources (swapchain backbuffer, persistent textures)
		RGTextureHandle ImportTexture(const char* name,
									  std::shared_ptr<Texture> texture,
									  ImageLayout currentLayout = ImageLayout::Undefined,
									  PixelFormat format = PixelFormat::RGBA8_SRGB,
									  UInt32 width = 0,
									  UInt32 height = 0);
		RGBufferHandle ImportBuffer(const char* name,
									std::shared_ptr<Buffer> buffer);

		// Pass declaration
		void AddPass(const char* name, RGPassType type,
					 std::function<void(RenderGraphBuilder&)> setup,
					 std::function<void(RenderGraphContext&)> execute);

		void AddGraphicsPass(const char* name,
							 std::function<void(RenderGraphBuilder&)> setup,
							 std::function<void(RenderGraphContext&)> execute);
		void AddComputePass(const char* name,
							std::function<void(RenderGraphBuilder&)> setup,
							std::function<void(RenderGraphContext&)> execute);
		void AddTransferPass(const char* name,
							 std::function<void(RenderGraphBuilder&)> setup,
							 std::function<void(RenderGraphContext&)> execute);

		void SetFinalOutput(RGTextureHandle handle);

		void Compile();

		// Compile (if dirty) and allocate physical resources for all declared transients.
		// Idempotent; called by Execute(). Call it explicitly to access transient textures
		// (GetTexture) before the first Execute — e.g. to bind descriptor sets once at build time.
		void AllocateResources();

		void Execute(CommandBuffer& cmd, UInt32 frameWidth, UInt32 frameHeight);
		void Reset();

		// Physical texture behind a handle. Transients require AllocateResources() first.
		// The assignment is stable until Clear(): descriptor sets bound to this texture
		// stay valid across frames.
		[[nodiscard]] Texture& GetTexture(RGTextureHandle handle);

		// Update the tracked layout of an imported texture after an external pipeline barrier.
		// Call this when you manually emit a barrier on a texture that bypasses the graph's tracking.
		void SetTextureLayout(RGTextureHandle handle, ImageLayout layout);

		// Declare the layout a texture must be in after the last pass that writes it.
		// Execute() emits the required barrier automatically at the end of the frame.
		void SetExportLayout(RGTextureHandle handle, ImageLayout layout);

		// Query the layout currently tracked by the registry for an imported texture.
		// Useful after Execute() to determine which textures need a post-pass barrier.
		[[nodiscard]] ImageLayout GetCurrentLayout(RGTextureHandle handle) const;

		// Force recompilation on next Execute (e.g. after window resize or pass topology change).
		void MarkDirty();

		// Clear all passes and resource registrations while preserving pending frame resources
		// (framebuffers, transients) so in-flight GPU work remains safe.
		// Call after Reset(), before re-declaring a new set of passes.
		void Clear();

		// Debug: write graph as text to the given stream.
		void DumpGraph(std::ostream& out) const;

	private:
		// Members are destroyed in reverse declaration order, so this list is ordered
		// dependencies-first: framebuffers must go before the views they reference,
		// and views before the images and render passes they were created from.
		struct PendingFrameResources
		{
			std::vector<RenderGraphResourceRegistry::BufferTransientSnapshot> transientBuffers;
			std::vector<RenderGraphResourceRegistry::TextureTransientSnapshot> transientTextures;
			std::vector<std::unique_ptr<RenderPass>> renderPasses;
			std::vector<std::unique_ptr<TextureView>> attachmentViews;
			std::vector<std::unique_ptr<FrameBuffer>> frameBuffers;
		};

		// Move GPU objects owned by the current compiled passes into m_retired,
		// so a recompile never destroys resources a previous frame may still use.
		void RetireCompiledPasses();

		Device& m_device;
		std::vector<RGPass> m_passes;
		RenderGraphResourceRegistry m_registry;
		RenderGraphCompiler m_compiler;
		std::vector<RGCompiledPass> m_compiledPasses;
		PendingFrameResources m_retired; // staged by Clear()/Compile(), drained into m_pendingFrames by Reset()
		std::deque<PendingFrameResources> m_pendingFrames; // deferred cleanup queue
		UInt32 m_maxFramesInFlight;
		RGTextureHandle m_finalOutput;
		bool m_dirty = true;
		FrameBuffer* m_currentFrameBuffer = nullptr; // active framebuffer across a merge group
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_RENDERGRAPH_HPP
