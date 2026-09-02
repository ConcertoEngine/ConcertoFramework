#ifndef CONCERTO_GRAPHICS_RHI_MATERILABUILDER_HPP
#define CONCERTO_GRAPHICS_RHI_MATERILABUILDER_HPP

#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Material.hpp"
#include "Concerto/Graphics/RHI/Material/MaterialInstance.hpp"
#include "Concerto/Graphics/RHI/Material/MaterialTemplate.hpp"

namespace cct::gfx::rhi
{
	class Device;
	class RenderPass;

	class CONCERTO_GRAPHICS_RHI_BASE_API MaterialBuilder
	{
	public:
		virtual ~MaterialBuilder() = default;

		virtual MaterialTemplatePtr BuildTemplate(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const RenderPass& renderPass, const PipelineConfig& pipelineConfig = {}) = 0;
		virtual MaterialInstancePtr Instantiate(const MaterialTemplatePtr& materialTemplate, const MaterialInfo& info) = 0;
		virtual MaterialInstancePtr BuildMaterial(const MaterialInfo& info, const RenderPass& renderPass) = 0;
		virtual void Update(const rhi::Buffer& buffer, UInt32 setIndex, UInt32 bindingIndex) = 0;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_MATERILABUILDER_HPP
