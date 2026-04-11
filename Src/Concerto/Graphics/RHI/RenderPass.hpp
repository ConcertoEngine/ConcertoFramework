//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_RENDERPASS_HPP
#define CONCERTO_GRAPHICS_RHI_RENDERPASS_HPP

#include <optional>
#include <vector>

#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API RenderPass
	{
	public:
		virtual ~RenderPass() = default;
		struct Attachment
		{
			PixelFormat pixelFormat;
			AttachmentLoadOp loadOp;
			AttachmentLoadOp stencilLoadOp;
			AttachmentStoreOp storeOp;
			AttachmentStoreOp stencilStoreOp;
			ImageLayout initialLayout;
			ImageLayout finalLayout;
		};

		struct SubPassDescription
		{
			std::vector<AttachmentReference> colorAttachments;
			std::vector<AttachmentReference> inputAttachments;
			std::vector<UInt32> preserveAttachments;
			std::optional<AttachmentReference> depthStencilAttachment;
		};

		struct SubPassDependency
		{
			static constexpr UInt32 ExternalSubPass = ~0U;

			UInt32 srcSubPassIndex;
			PipelineStageFlags srcStageMask;
			MemoryAccessFlags srcAccessFlags;

			UInt32 dstSubPassIndex;
			PipelineStageFlags dstStageMask;
			MemoryAccessFlags dstAccessFlags;
		};
	};
}

#endif //CONCERTO_GRAPHICS_RHI_RENDERPASS_HPP