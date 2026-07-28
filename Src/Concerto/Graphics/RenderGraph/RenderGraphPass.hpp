//
// Created by arthur on 11/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_RENDERGRAPH_PASS_HPP
#define CONCERTO_GRAPHICS_RHI_RENDERGRAPH_PASS_HPP

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Concerto/Graphics/RenderGraph/RenderGraphResource.hpp"
#include "Concerto/Graphics/RHI/FrameBuffer.hpp"
#include "Concerto/Graphics/RHI/RenderPass.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"

namespace cct::gfx::rhi
{
	class RenderGraphContext;

	struct RGTextureUsage
	{
		RGTextureHandle handle;
		RGResourceAccess access;
		bool isDepth = false;
	};

	struct RGBufferUsage
	{
		RGBufferHandle handle;
		RGResourceAccess access;
	};

	struct RGPass
	{
		std::string name;
		RGPassType type;
		std::vector<RGTextureUsage> textureUsages;
		std::vector<RGBufferUsage> bufferUsages;
		std::vector<RGTextureHandle> colorAttachments;
		std::optional<RGTextureHandle> depthAttachment;
		std::function<void(RenderGraphContext&)> execute;
		bool culled = false;
		bool sideEffects = false;
		UInt32 refCount = 0;
	};

	struct RGCompiledPass
	{
		UInt32 passIndex;
		std::unique_ptr<RenderPass> renderPass; // non-null when this pass opens a new render pass
		bool closesRenderPass = true; // false when the next pass continues the same render pass

		// Declared so the framebuffer is destroyed before the views it references
		// (members are destroyed in reverse declaration order).
		std::vector<std::unique_ptr<TextureView>> attachmentViews;
		std::unique_ptr<FrameBuffer> frameBuffer; // cached, created lazily on first Execute after Compile
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_RENDERGRAPH_PASS_HPP
