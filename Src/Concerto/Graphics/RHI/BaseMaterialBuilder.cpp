#include "Concerto/Graphics/RHI/BaseMaterialBuilder.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>

#include "Concerto/Graphics/Core/Vertex.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/DescriptorSet.hpp"
#include "Concerto/Graphics/RHI/DescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Pipeline.hpp"
#include "Concerto/Graphics/RHI/PipelineLayout.hpp"
#include "Concerto/Graphics/RHI/RenderPass.hpp"
#include "Concerto/Graphics/RHI/TextureBuilder/TextureBuilder.hpp"

namespace
{
	cct::UInt64 HashShaderPair(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const cct::gfx::rhi::PipelineConfig& config)
	{
		std::hash<std::string> stringHasher;
		std::hash<int> intHasher;
		cct::UInt64 hash = 0;
		hash ^= stringHasher(vertexShaderPath) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= stringHasher(fragmentShaderPath) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= intHasher(config.blendEnable ? 1 : 0) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= intHasher(config.premultipliedAlpha ? 1 : 0) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= intHasher(static_cast<int>(config.blendPreset)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= intHasher(config.depthTestEnable ? 1 : 0) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= intHasher(config.depthWriteEnable ? 1 : 0) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}

	cct::gfx::rhi::PipelineConfig BuildVertexPipelineConfig(const cct::gfx::rhi::PipelineConfig& materialConfig)
	{
		using cct::gfx::rhi::VertexAttribute;
		using cct::gfx::rhi::VertexAttributeFormat;

		cct::gfx::rhi::PipelineConfig config = materialConfig;
		config.vertexStride = static_cast<cct::UInt32>(sizeof(cct::gfx::Vertex));
		config.vertexAttributes = {
			VertexAttribute{0, VertexAttributeFormat::Vec3f, static_cast<cct::UInt32>(offsetof(cct::gfx::Vertex, position))},
			VertexAttribute{1, VertexAttributeFormat::Vec3f, static_cast<cct::UInt32>(offsetof(cct::gfx::Vertex, normal))},
			VertexAttribute{2, VertexAttributeFormat::Vec3f, static_cast<cct::UInt32>(offsetof(cct::gfx::Vertex, color))},
			VertexAttribute{3, VertexAttributeFormat::Vec2f, static_cast<cct::UInt32>(offsetof(cct::gfx::Vertex, uv))},
		};
		return config;
	}
} // namespace

namespace cct::gfx::rhi
{
	BaseMaterialBuilder::BaseMaterialBuilder(Device& device, const Vector2u& windowExtent) :
		m_device(device),
		m_windowExtent(windowExtent)
	{
	}

	rhi::ShaderModule& BaseMaterialBuilder::GetOrLoadShaderModule(const std::string& path)
	{
		CCT_AUTO_PROFILER_SCOPE();

		auto it = m_shaderModules.find(path);
		if (it == m_shaderModules.end())
		{
			std::shared_ptr<rhi::ShaderModule> shaderModule = m_device.CreateShaderModule(path);
			auto [iter, inserted] = m_shaderModules.emplace(path, shaderModule);
			return *iter->second;
		}
		return *it->second;
	}

	std::unordered_map<UInt32, std::vector<cct::gfx::DescriptorSetLayoutBinding>> BaseMaterialBuilder::MergeBindings(
		const rhi::ShaderModule& vertexShader,
		const rhi::ShaderModule& fragmentShader)
	{
		CCT_AUTO_PROFILER_SCOPE();

		std::unordered_map<UInt32, std::vector<cct::gfx::DescriptorSetLayoutBinding>> merged(vertexShader.GetBindings());

		for (const auto& [set, setBindings] : fragmentShader.GetBindings())
		{
			auto setIt = merged.find(set);
			if (setIt == merged.end())
			{
				merged.emplace(set, setBindings);
			}
			else
			{
				for (const auto& fragBinding : setBindings)
				{
					auto bindingIt = std::find_if(setIt->second.begin(), setIt->second.end(),
												  [&fragBinding](const cct::gfx::DescriptorSetLayoutBinding& existing)
												  {
													  return existing.binding == fragBinding.binding;
												  });

					if (bindingIt != setIt->second.end())
					{
						bindingIt->stageFlags = bindingIt->stageFlags | fragBinding.stageFlags;
					}
					else
					{
						setIt->second.push_back(fragBinding);
					}
				}
			}
		}
		return merged;
	}

	MaterialTemplatePtr BaseMaterialBuilder::BuildTemplate(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const rhi::RenderPass& renderPass, const rhi::PipelineConfig& pipelineConfig)
	{
		CCT_AUTO_PROFILER_SCOPE();

		UInt64 templateHash = HashShaderPair(vertexShaderPath, fragmentShaderPath, pipelineConfig);
		if (auto it = m_templatesCache.find(templateHash); it != m_templatesCache.end())
			return it->second;

		auto& vertexShader = GetOrLoadShaderModule(vertexShaderPath);
		auto& fragmentShader = GetOrLoadShaderModule(fragmentShaderPath);

		auto mergedBindings = MergeBindings(vertexShader, fragmentShader);

		UInt32 maxSet = 0;
		for (const auto& [set, bindings] : mergedBindings)
			maxSet = std::max(maxSet, set);

		UInt64 emptyLayoutHash = 0;
		std::shared_ptr<rhi::DescriptorSetLayout> emptyLayout;
		if (auto emptyIt = m_descriptorSetLayoutsCache.find(emptyLayoutHash); emptyIt != m_descriptorSetLayoutsCache.end())
		{
			emptyLayout = emptyIt->second;
		}
		else
		{
			emptyLayout = m_device.CreateDescriptorSetLayout({});
			m_descriptorSetLayoutsCache.emplace(emptyLayoutHash, emptyLayout);
		}

		std::vector<std::shared_ptr<rhi::DescriptorSetLayout>> orderedLayouts(static_cast<std::size_t>(maxSet) + 1, emptyLayout);
		for (const auto& [set, bindings] : mergedBindings)
		{
			std::hash<UInt32> hasher;
			UInt64 layoutHash = 0;
			for (const auto& binding : bindings)
			{
				layoutHash ^= hasher(binding.binding) + 0x9e3779b9 + (layoutHash << 6) + (layoutHash >> 2);
				layoutHash ^= hasher(static_cast<UInt32>(binding.descriptorType)) + 0x9e3779b9 + (layoutHash << 6) + (layoutHash >> 2);
				layoutHash ^= hasher(binding.descriptorCount) + 0x9e3779b9 + (layoutHash << 6) + (layoutHash >> 2);
				layoutHash ^= hasher(binding.stageFlags.Value()) + 0x9e3779b9 + (layoutHash << 6) + (layoutHash >> 2);
			}
			auto layoutIt = m_descriptorSetLayoutsCache.find(layoutHash);
			if (layoutIt != m_descriptorSetLayoutsCache.end())
			{
				orderedLayouts[set] = layoutIt->second;
			}
			else
			{
				auto layout = m_device.CreateDescriptorSetLayout(bindings);
				m_descriptorSetLayoutsCache.emplace(layoutHash, layout);
				orderedLayouts[set] = layout;
			}
		}

		auto materialTemplate = std::make_shared<MaterialTemplate>();
		materialTemplate->vertexShaderPath = vertexShaderPath;
		materialTemplate->fragmentShaderPath = fragmentShaderPath;
		materialTemplate->materialParams = fragmentShader.GetMaterialParamsLayout().size > 0
											   ? fragmentShader.GetMaterialParamsLayout()
											   : vertexShader.GetMaterialParamsLayout();
		materialTemplate->descriptorSetLayouts.reserve(orderedLayouts.size());
		for (std::size_t i = 0; i < orderedLayouts.size(); ++i)
		{
			auto& layout = orderedLayouts[i];
			for (const auto& binding : layout->GetBindings())
			{
				if (binding.descriptorType == cct::gfx::ShaderBindingType::Sampler)
				{
					materialTemplate->diffuseTextureSetIndex = static_cast<UInt32>(i);
					materialTemplate->diffuseTextureBinding = binding.binding;
				}
			}
			materialTemplate->descriptorSetLayouts.push_back(std::move(layout));
		}

		materialTemplate->pipelineConfig = BuildVertexPipelineConfig(pipelineConfig);

		auto pipelineLayout = m_device.CreatePipelineLayout(materialTemplate->descriptorSetLayouts);
		materialTemplate->pipeline = m_device.CreatePipeline(vertexShader, fragmentShader, renderPass, *pipelineLayout, m_windowExtent, materialTemplate->pipelineConfig);
		materialTemplate->pipelineLayout = materialTemplate->pipeline->GetPipelineLayout();

		m_templatesCache.emplace(templateHash, materialTemplate);
		return materialTemplate;
	}

	MaterialInstancePtr BaseMaterialBuilder::Instantiate(const MaterialTemplatePtr& materialTemplate, const rhi::MaterialInfo& info)
	{
		CCT_AUTO_PROFILER_SCOPE();

		auto instance = std::make_shared<MaterialInstance>();
		instance->info = info;
		instance->materialTemplate = materialTemplate;

		instance->descriptorSets.reserve(materialTemplate->descriptorSetLayouts.size());
		for (const auto& layout : materialTemplate->descriptorSetLayouts)
			instance->descriptorSets.push_back(std::shared_ptr<DescriptorSet>(m_device.CreateDescriptorSet(*layout)));

		if (!info.diffuseTexturePath.empty())
		{
			instance->diffuseTexture = TextureBuilder::Instance().BuildTexture(info.diffuseTexturePath);

			if (materialTemplate->diffuseTextureSetIndex != std::numeric_limits<UInt32>::max() &&
				materialTemplate->diffuseTextureSetIndex < instance->descriptorSets.size())
			{
				instance->descriptorSets[materialTemplate->diffuseTextureSetIndex]->BindTexture(materialTemplate->diffuseTextureBinding, *instance->diffuseTexture);
			}
		}

		if (materialTemplate->materialParams.size > 0)
		{
			instance->valueData.assign(materialTemplate->materialParams.size, std::byte{0});

			instance->SetValue("diffuseColor", info.diffuseColor);
			instance->SetValue("metallic", info.metallic);
			instance->SetValue("roughness", info.roughness);
			instance->SetValue("specular", info.specular);
			instance->SetValue("anisotropy", info.anisotropy);
			instance->SetValue("emissiveColor", info.emissiveColor);

			instance->valueBuffer = m_device.CreateBuffer(rhi::BufferUsage::Uniform, static_cast<UInt32>(materialTemplate->materialParams.size), true);
			instance->UploadValues();
		}

		return instance;
	}

	MaterialInstancePtr BaseMaterialBuilder::BuildMaterial(const rhi::MaterialInfo& info, const rhi::RenderPass& renderPass)
	{
		CCT_AUTO_PROFILER_SCOPE();

		auto materialTemplate = BuildTemplate(info.vertexShaderPath, info.fragmentShaderPath, renderPass, info.pipelineConfig);

		UInt64 templateHash = HashShaderPair(info.vertexShaderPath, info.fragmentShaderPath, info.pipelineConfig);
		std::size_t instanceHash = static_cast<std::size_t>(templateHash) ^ (info.GetHash() + 0x9e3779b9 + (static_cast<std::size_t>(templateHash) << 6) + (static_cast<std::size_t>(templateHash) >> 2));

		if (auto it = m_instancesCache.find(instanceHash); it != m_instancesCache.end() && it->second->info == info)
			return it->second;

		auto instance = Instantiate(materialTemplate, info);
		m_instancesCache.insert_or_assign(instanceHash, instance);
		return instance;
	}

	void BaseMaterialBuilder::Update(const rhi::Buffer& buffer, UInt32 setIndex, UInt32 bindingIndex)
	{
		CCT_AUTO_PROFILER_SCOPE();

		for (const auto& [hash, instance] : m_instancesCache)
		{
			if (setIndex < instance->descriptorSets.size())
			{
				CCT_ASSERT(instance->descriptorSets[setIndex], "Invalid pointer");
				instance->descriptorSets[setIndex]->BindBuffer(bindingIndex, buffer);
			}
		}
	}
} // namespace cct::gfx::rhi
