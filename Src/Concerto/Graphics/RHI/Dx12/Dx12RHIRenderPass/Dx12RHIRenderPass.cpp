//
// Created by arthur on 22/03/2026.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIRenderPass/Dx12RHIRenderPass.hpp"

#include "Concerto/Graphics/Backend/Dx12/Wrapper/Factory/Factory.hpp"

namespace cct::gfx::rhi
{
	Dx12RHIRenderPass::Dx12RHIRenderPass(std::span<RenderPass::Attachment> attachments,
										 std::span<RenderPass::SubPassDescription> subPassDescriptions,
										 std::span<RenderPass::SubPassDependency> subPassDependencies) :
		m_attachments(attachments.begin(), attachments.end()),
		m_subPassDescriptions(subPassDescriptions.begin(), subPassDescriptions.end()),
		m_subPassDependencies(subPassDependencies.begin(), subPassDependencies.end())
	{
	}

	DXGI_FORMAT Dx12RHIRenderPass::GetColorAttachmentFormat() const
	{
		for (const auto& attachment : m_attachments)
		{
			if (attachment.finalLayout != ImageLayout::DepthStencilAttachmentOptimal)
				return dx12::Factory::PixelFormatToDXGI(attachment.pixelFormat);
		}
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	}

	DXGI_FORMAT Dx12RHIRenderPass::GetDepthAttachmentFormat() const
	{
		for (const auto& attachment : m_attachments)
		{
			if (attachment.finalLayout == ImageLayout::DepthStencilAttachmentOptimal)
				return dx12::Factory::PixelFormatToDXGI(attachment.pixelFormat);
		}
		return DXGI_FORMAT_UNKNOWN;
	}
} // namespace cct::gfx::rhi
