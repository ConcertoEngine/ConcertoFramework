#ifndef CONCERTO_GRAPHICS_RHI_BASEMATERIALBUILDER_HPP
#define CONCERTO_GRAPHICS_RHI_BASEMATERIALBUILDER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "Concerto/Graphics/Core/ShaderModule/ShaderModule.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/MaterialBuilder.hpp"
#include "Concerto/Graphics/RHI/ShaderModule.hpp"

namespace cct::gfx::rhi
{
	class Device;
	class DescriptorSetLayout;
	class Pipeline;

	class CONCERTO_GRAPHICS_RHI_BASE_API BaseMaterialBuilder : public MaterialBuilder
	{
	public:
		explicit BaseMaterialBuilder(Device& device, const Vector2u& windowExtent);
		~BaseMaterialBuilder() override = default;

		MaterialTemplatePtr BuildTemplate(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const rhi::RenderPass& renderPass, const rhi::PipelineConfig& pipelineConfig) override;
		MaterialInstancePtr Instantiate(const MaterialTemplatePtr& materialTemplate, const rhi::MaterialInfo& info) override;
		MaterialInstancePtr BuildMaterial(const rhi::MaterialInfo& info, const rhi::RenderPass& renderPass) override;
		void Update(const rhi::Buffer& buffer, UInt32 setIndex, UInt32 bindingIndex) override;

	protected:
		rhi::ShaderModule& GetOrLoadShaderModule(const std::string& path);

		std::unordered_map<UInt32, std::vector<cct::gfx::DescriptorSetLayoutBinding>> MergeBindings(
			const rhi::ShaderModule& vertexShader,
			const rhi::ShaderModule& fragmentShader);

		Device& m_device;
		Vector2u m_windowExtent;
		ThreadSafeHashMap<std::string, std::shared_ptr<rhi::ShaderModule>> m_shaderModules;
		ThreadSafeHashMap<UInt64, MaterialTemplatePtr> m_templatesCache;
		ThreadSafeHashMap<UInt64, std::shared_ptr<rhi::DescriptorSetLayout>> m_descriptorSetLayoutsCache;
		ThreadSafeHashMap<std::size_t, MaterialInstancePtr> m_instancesCache;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_BASEMATERIALBUILDER_HPP
