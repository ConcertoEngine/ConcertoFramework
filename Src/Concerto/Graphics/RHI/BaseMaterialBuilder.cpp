//
// Created by arthur on 23/10/2025.
//

#include "Concerto/Graphics/RHI/BaseMaterialBuilder.hpp"

#include <algorithm>

#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/DescriptorSet.hpp"
#include "Concerto/Graphics/RHI/DescriptorSetLayout.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"
#include "Concerto/Graphics/RHI/Pipeline.hpp"
#include "Concerto/Graphics/RHI/PipelineLayout.hpp"
#include "Concerto/Graphics/RHI/RenderPass.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipeline/VkRHIPipeline.hpp"
#include "Concerto/Graphics/RHI/Vulkan/VkRHIPipelineLayout/VkRHIPipelineLayout.hpp"

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
				// Set doesn't exist yet, add all bindings
				merged.emplace(set, setBindings);
			}
			else
			{
				// Set exists, need to merge bindings carefully
				for (const auto& fragBinding : setBindings)
				{
					// Look for existing binding with same binding number
					auto bindingIt = std::find_if(setIt->second.begin(), setIt->second.end(),
												  [&fragBinding](const cct::gfx::DescriptorSetLayoutBinding& existing)
												  {
													  return existing.binding == fragBinding.binding;
												  });

					if (bindingIt != setIt->second.end())
					{
						// Binding exists, merge stage flags by ORing them
						bindingIt->stageFlags = bindingIt->stageFlags | fragBinding.stageFlags;
					}
					else
					{
						// New binding, add it
						setIt->second.push_back(fragBinding);
					}
				}
			}
		}
		return merged;
	}

	MaterialPtr BaseMaterialBuilder::BuildMaterial(rhi::MaterialInfo& material, const rhi::RenderPass& renderPass)
	{
		CCT_AUTO_PROFILER_SCOPE();

		// Load shaders
		auto& vertexShader = GetOrLoadShaderModule(material.vertexShaderPath);
		auto& fragmentShader = GetOrLoadShaderModule(material.fragmentShaderPath);

		// Merge bindings from both shaders
		auto mergedBindings = MergeBindings(vertexShader, fragmentShader);

		// Create descriptor set layouts from merged bindings
		std::vector<std::pair<UInt32, std::shared_ptr<rhi::DescriptorSetLayout>>> layoutEntries;
		layoutEntries.reserve(mergedBindings.size());
		for (const auto& [set, bindings] : mergedBindings)
		{
			// Try to find in cache
			std::hash<UInt32> hasher;
			UInt64 hash = 0;
			for (const auto& binding : bindings)
			{
				hash ^= hasher(binding.binding) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				hash ^= hasher(static_cast<UInt32>(binding.descriptorType)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				hash ^= hasher(binding.descriptorCount) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				hash ^= hasher(binding.stageFlags.Value()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			}
			auto it = m_descriptorSetLayoutsCache.find(hash);
			if (it != m_descriptorSetLayoutsCache.end())
			{
				layoutEntries.emplace_back(set, it->second);
			}
			else
			{
				auto layout = m_device.CreateDescriptorSetLayout(bindings);
				m_descriptorSetLayoutsCache.emplace(hash, layout);
				layoutEntries.emplace_back(set, layout);
			}
		}

		// Sort layouts by set index to match Vulkan expectations (set 0 first, then set 1, etc.)
		std::sort(layoutEntries.begin(), layoutEntries.end(), [](const auto& lhs, const auto& rhs)
				  { return lhs.first < rhs.first; });

		std::vector<std::shared_ptr<rhi::DescriptorSetLayout>> descriptorSetLayouts;
		descriptorSetLayouts.reserve(layoutEntries.size());
		for (auto& entry : layoutEntries)
			descriptorSetLayouts.push_back(std::move(entry.second));

		auto pipelineLayout = m_device.CreatePipelineLayout(descriptorSetLayouts);

		// Create or get cached pipeline
		UInt64 pipelineHash = 0;
		std::hash<std::string> hasher;
		pipelineHash ^= hasher(material.vertexShaderPath) + 0x9e3779b9 + (pipelineHash << 6) + (pipelineHash >> 2);
		pipelineHash ^= hasher(material.fragmentShaderPath) + 0x9e3779b9 + (pipelineHash << 6) + (pipelineHash >> 2);

		// Check cache first to avoid creating duplicate pipelines
		auto pipelineIt = m_pipelinesCache.find(pipelineHash);
		if (pipelineIt == m_pipelinesCache.end())
		{
			auto pipeline = m_device.CreatePipeline(vertexShader, fragmentShader, renderPass, *pipelineLayout, m_windowExtent);
			pipelineIt = m_pipelinesCache.insert_or_assign(pipelineHash, pipeline).first;
		}

		auto materialPtr = std::make_shared<Material>(material);
		materialPtr->pipeline = pipelineIt->second;
		if (auto vkPipeline = std::dynamic_pointer_cast<rhi::VkRHIPipeline>(pipelineIt->second))
			materialPtr->pipelineLayout = std::static_pointer_cast<PipelineLayout>(vkPipeline->GetLayoutPtr());
		else
			materialPtr->pipelineLayout = pipelineLayout;

		materialPtr->descriptorSets.reserve(descriptorSetLayouts.size());
		UInt32 textureSetIndex = std::numeric_limits<UInt32>::max();
		UInt32 textureBinding = 0;

		for (std::size_t i = 0; i < descriptorSetLayouts.size(); ++i)
		{
			auto descriptorSet = m_device.CreateDescriptorSet(*descriptorSetLayouts[i]);

			// Check if this set contains a texture binding
			for (const auto& binding : descriptorSetLayouts[i]->GetBindings())
			{
				if (binding.descriptorType == cct::gfx::ShaderBindingType::Sampler)
				{
					textureSetIndex = static_cast<UInt32>(i);
					textureBinding = binding.binding;
				}
			}

			// Convert unique_ptr to shared_ptr
			materialPtr->descriptorSets.push_back(std::shared_ptr<DescriptorSet>(std::move(descriptorSet)));
		}

		// Load and bind texture if present
		if (!material.diffuseTexturePath.empty())
		{
			materialPtr->diffuseTexture = TextureBuilder::Instance().BuildTexture(material.diffuseTexturePath);

			// Bind texture to descriptor set if we found a texture binding
			if (textureSetIndex != std::numeric_limits<UInt32>::max() && textureSetIndex < materialPtr->descriptorSets.size())
			{
				materialPtr->descriptorSets[textureSetIndex]->BindTexture(textureBinding, *materialPtr->diffuseTexture);
			}
		}

		// Cache material for Update()
		m_materialsCache.emplace(materialPtr);

		return materialPtr;
	}

	void BaseMaterialBuilder::Update(const rhi::Buffer& buffer, UInt32 setIndex, UInt32 bindingIndex)
	{
		CCT_AUTO_PROFILER_SCOPE();

		// Update all materials' descriptor sets with the new buffer
		for (const auto& material : m_materialsCache)
		{
			if (setIndex < material->descriptorSets.size())
			{
				CCT_ASSERT(material->descriptorSets[setIndex], "Invalid pointer");
				material->descriptorSets[setIndex]->BindBuffer(bindingIndex, buffer);
			}
		}
	}
} // namespace cct::gfx::rhi
