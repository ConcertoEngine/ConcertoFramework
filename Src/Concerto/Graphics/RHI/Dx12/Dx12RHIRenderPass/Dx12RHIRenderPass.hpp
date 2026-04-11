//
// Created by arthur on 22/03/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHIRENDERPASS_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHIRENDERPASS_HPP

#include <span>
#include <vector>

#include "Concerto/Graphics/RHI/RenderPass.hpp"
#include "Concerto/Graphics/Backend/Dx12/Defines.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIRenderPass : public rhi::RenderPass
	{
	public:
		Dx12RHIRenderPass(std::span<RenderPass::Attachment> attachments,
		                  std::span<RenderPass::SubPassDescription> subPassDescriptions,
		                  std::span<RenderPass::SubPassDependency> subPassDependencies);

		[[nodiscard]] const std::vector<Attachment>& GetAttachments() const { return m_attachments; }
		[[nodiscard]] const std::vector<SubPassDescription>& GetSubPassDescriptions() const { return m_subPassDescriptions; }
		[[nodiscard]] const std::vector<SubPassDependency>& GetSubPassDependencies() const { return m_subPassDependencies; }

		[[nodiscard]] DXGI_FORMAT GetColorAttachmentFormat() const;
		[[nodiscard]] DXGI_FORMAT GetDepthAttachmentFormat() const;

	private:
		std::vector<Attachment> m_attachments;
		std::vector<SubPassDescription> m_subPassDescriptions;
		std::vector<SubPassDependency> m_subPassDependencies;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_DX12_DX12RHIRENDERPASS_HPP
