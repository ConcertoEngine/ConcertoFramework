#ifndef CONCERTO_GRAPHICS_RHI_MATERIAL_MATERIALTEMPLATE_HPP
#define CONCERTO_GRAPHICS_RHI_MATERIAL_MATERIALTEMPLATE_HPP

#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "Concerto/Graphics/Core/ShaderModule/ShaderModule.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"

namespace cct::gfx::rhi
{
	class Pipeline;
	class PipelineLayout;
	class DescriptorSetLayout;

	class CONCERTO_GRAPHICS_RHI_BASE_API MaterialTemplate
	{
	public:
		std::string vertexShaderPath;
		std::string fragmentShaderPath;
		PipelineConfig pipelineConfig;
		std::shared_ptr<Pipeline> pipeline;
		std::shared_ptr<PipelineLayout> pipelineLayout;
		std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts;
		UInt32 diffuseTextureSetIndex = std::numeric_limits<UInt32>::max();
		UInt32 diffuseTextureBinding = 0;
		cct::gfx::MaterialParamsLayout materialParams;

		[[nodiscard]] const cct::gfx::MaterialValueProperty* FindValueProperty(const std::string& name) const
		{
			for (const auto& property : materialParams.properties)
			{
				if (property.name == name)
					return &property;
			}
			return nullptr;
		}
	};
	using MaterialTemplatePtr = std::shared_ptr<MaterialTemplate>;
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_MATERIAL_MATERIALTEMPLATE_HPP
