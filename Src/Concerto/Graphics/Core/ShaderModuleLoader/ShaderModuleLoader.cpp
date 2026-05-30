//
// Created by arthur on 23/10/2025.
//

#include "Concerto/Graphics/Core/ShaderModuleLoader/ShaderModuleLoader.hpp"

#include <ranges>

#include <Concerto/Core/Assert.hpp>

#include <NZSL/Ast/Cloner.hpp>
#include <NZSL/Ast/ReflectVisitor.hpp>
#include <NZSL/Ast/Transformations/BindingResolverTransformer.hpp>
#include <NZSL/Ast/Transformations/ResolveTransformer.hpp>
#include <NZSL/Ast/Transformations/ValidationTransformer.hpp>
#include <NZSL/Ast/TransformerExecutor.hpp>
#include <NZSL/FilesystemModuleResolver.hpp>
#include <NZSL/Parser.hpp>
#include <NZSL/SpirvWriter.hpp>

namespace cct::gfx
{
	namespace
	{
		ShaderStage ToShaderStage(nzsl::ShaderStageType stageType)
		{
			switch (stageType)
			{
				case nzsl::ShaderStageType::Vertex:
					return ShaderStage::Vertex;
				case nzsl::ShaderStageType::Fragment:
					return ShaderStage::Fragment;
				case nzsl::ShaderStageType::Compute:
					return ShaderStage::Compute;
			}
			CCT_ASSERT_FALSE("ConcertoGraphics: Unexpected shader stage type");
			return ShaderStage::Vertex;
		}
	} // namespace

	ResolvedShaderModule ShaderModuleLoader::ResolveShaderModule(const std::string& path,
																 ShaderStage stageFilter)
	{
		nzsl::Ast::ModulePtr shaderAst = nzsl::ParseFromFile(path);

		nzsl::Ast::TransformerExecutor executor;

		nzsl::Ast::ResolveTransformer::Options resolveOpts;
		if (!m_modulePath.empty())
		{
			auto resolver = std::make_shared<nzsl::FilesystemModuleResolver>();
			resolver->RegisterDirectory(m_modulePath);
			resolveOpts.moduleResolver = std::move(resolver);
		}
		executor.AddPass<nzsl::Ast::ResolveTransformer>(resolveOpts);
		executor.AddPass<nzsl::Ast::BindingResolverTransformer>({.forceAutoBindingResolve = true});
		executor.AddPass<nzsl::Ast::ValidationTransformer>();

		nzsl::Ast::TransformerContext context;
		context.partialCompilation = true;

		nzsl::Ast::ModulePtr resolvedModule = nzsl::Ast::Clone(*shaderAst);
		executor.Transform(*resolvedModule, context);

		ResolvedShaderModule resolved;
		resolved.stage = ShaderStage::Vertex;

		nzsl::Ast::ReflectVisitor reflectVisitor;
		nzsl::Ast::ReflectVisitor::Callbacks callbacks;
		callbacks.onEntryPointDeclaration = [&](nzsl::ShaderStageType stageType, const std::string& functionName)
		{
			ShaderStage stage = ToShaderStage(stageType);
			if (stageFilter == ShaderStage::None || stage == stageFilter)
			{
				resolved.stage = stage;
				resolved.entryPointName = functionName;
			}
		};

		callbacks.onExternalDeclaration = [&](const nzsl::Ast::DeclareExternalStatement& extDecl)
		{
			for (auto& externalVariable : extDecl.externalVars)
			{
				const auto* varType = &externalVariable.type.GetResultingValue();
				const ShaderBindingType descriptorType = GetBindingType(varType);
				UInt32 bindingSet = externalVariable.bindingSet.GetResultingValue();

				DescriptorSetLayoutBinding descriptorSetLayoutBinding;
				descriptorSetLayoutBinding.binding = externalVariable.bindingIndex.GetResultingValue();
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.descriptorType = descriptorType;
				descriptorSetLayoutBinding.stageFlags = resolved.stage;

				auto layoutBindings = resolved.bindings.find(bindingSet);
				if (layoutBindings == resolved.bindings.end())
					resolved.bindings[bindingSet] = std::vector{descriptorSetLayoutBinding};
				else
					layoutBindings->second.push_back(descriptorSetLayoutBinding);
			}
		};

		reflectVisitor.Reflect(*resolvedModule, callbacks);

		for (auto& b : resolved.bindings | std::views::values)
		{
			for (auto& binding : b)
				binding.stageFlags = resolved.stage;
		}

		resolved.resolvedAst = std::move(resolvedModule);
		return resolved;
	}

	ShaderModule ShaderModuleLoader::LoadShaderModule(const std::string& path, ShaderStage stageFilter)
	{
		auto resolved = ResolveShaderModule(path, stageFilter);

		nzsl::SpirvWriter spirvWriter;
		nzsl::SpirvWriter::Environment env = {
			.spvMajorVersion = 1,
			.spvMinorVersion = 3};
		spirvWriter.SetEnv(env);
		std::vector<UInt32> spirv = spirvWriter.Generate(*resolved.resolvedAst);

		return ShaderModule(std::move(spirv), std::move(resolved.bindings), std::move(resolved.entryPointName), resolved.stage);
	}

	ResolvedShaderModule ShaderModuleLoader::ResolveShaderModuleFromSource(std::string_view source,
																		   std::string_view label,
																		   ShaderStage stageFilter)
	{
		try
		{
			nzsl::Ast::ModulePtr shaderAst = nzsl::Parse(source, std::string(label));

			nzsl::Ast::TransformerExecutor executor;

			nzsl::Ast::ResolveTransformer::Options resolveOpts;
			if (!m_modulePath.empty())
			{
				auto resolver = std::make_shared<nzsl::FilesystemModuleResolver>();
				resolver->RegisterDirectory(m_modulePath);
				resolveOpts.moduleResolver = std::move(resolver);
			}
			executor.AddPass<nzsl::Ast::ResolveTransformer>(resolveOpts);
			executor.AddPass<nzsl::Ast::BindingResolverTransformer>({.forceAutoBindingResolve = true});
			executor.AddPass<nzsl::Ast::ValidationTransformer>();

			nzsl::Ast::TransformerContext context;
			context.partialCompilation = true;

			nzsl::Ast::ModulePtr resolvedModule = nzsl::Ast::Clone(*shaderAst);
			executor.Transform(*resolvedModule, context);

			ResolvedShaderModule resolved;
			resolved.stage = ShaderStage::Vertex;

			nzsl::Ast::ReflectVisitor reflectVisitor;
			nzsl::Ast::ReflectVisitor::Callbacks callbacks;
			callbacks.onEntryPointDeclaration = [&](nzsl::ShaderStageType stageType, const std::string& functionName)
			{
				ShaderStage stage = ToShaderStage(stageType);
				if (stageFilter == ShaderStage::None || stage == stageFilter)
				{
					resolved.stage = stage;
					resolved.entryPointName = functionName;
				}
			};

			callbacks.onExternalDeclaration = [&](const nzsl::Ast::DeclareExternalStatement& extDecl)
			{
				for (auto& externalVariable : extDecl.externalVars)
				{
					const auto* varType = &externalVariable.type.GetResultingValue();
					const ShaderBindingType descriptorType = GetBindingType(varType);
					UInt32 bindingSet = externalVariable.bindingSet.GetResultingValue();

					DescriptorSetLayoutBinding descriptorSetLayoutBinding;
					descriptorSetLayoutBinding.binding = externalVariable.bindingIndex.GetResultingValue();
					descriptorSetLayoutBinding.descriptorCount = 1;
					descriptorSetLayoutBinding.descriptorType = descriptorType;
					descriptorSetLayoutBinding.stageFlags = resolved.stage;

					auto layoutBindings = resolved.bindings.find(bindingSet);
					if (layoutBindings == resolved.bindings.end())
						resolved.bindings[bindingSet] = std::vector{descriptorSetLayoutBinding};
					else
						layoutBindings->second.push_back(descriptorSetLayoutBinding);
				}
			};

			reflectVisitor.Reflect(*resolvedModule, callbacks);

			for (auto& b : resolved.bindings | std::views::values)
			{
				for (auto& binding : b)
					binding.stageFlags = resolved.stage;
			}

			resolved.resolvedAst = std::move(resolvedModule);
			return resolved;
		}
		catch (const nzsl::Error& e)
		{
			CCT_GFX_LOG_ERROR("ShaderModuleLoader", "Shader:\n{}", source);
			CCT_GFX_LOG_ERROR("ShaderModuleLoader", "NZSL error: {}", e.GetFullErrorMessage());
		}
		catch (const std::exception& e)
		{
			CCT_GFX_LOG_ERROR("ShaderModuleLoader", "NZSL", "{}", e.what());
		}
		return {};
	}

	ShaderModule ShaderModuleLoader::LoadShaderModuleFromSource(std::string_view source,
																std::string_view label,
																ShaderStage stageFilter)
	{
		auto resolved = ResolveShaderModuleFromSource(source, label, stageFilter);

		nzsl::SpirvWriter spirvWriter;
		nzsl::SpirvWriter::Environment env = {
			.spvMajorVersion = 1,
			.spvMinorVersion = 3};
		spirvWriter.SetEnv(env);
		std::vector<UInt32> spirv = spirvWriter.Generate(*resolved.resolvedAst);

		return ShaderModule(std::move(spirv), std::move(resolved.bindings), std::move(resolved.entryPointName), resolved.stage);
	}

	ShaderBindingType ShaderModuleLoader::GetBindingType(const nzsl::Ast::ExpressionType* varType)
	{
		if (nzsl::Ast::IsStorageType(*varType))
		{
			const auto& storageType = std::get<nzsl::Ast::StorageType>(*varType);
			if (storageType.accessPolicy == nzsl::AccessPolicy::ReadOnly)
				return ShaderBindingType::ReadOnlyStorageBuffer;
			return ShaderBindingType::StorageBuffer;
		}
		if (nzsl::Ast::IsSamplerType(*varType))
			return ShaderBindingType::Sampler;
		if (nzsl::Ast::IsTextureType(*varType))
			return ShaderBindingType::Texture;
		if (nzsl::Ast::IsUniformType(*varType))
			return ShaderBindingType::UniformBuffer;
		CCT_ASSERT_FALSE("ConcertoGraphics: Unexpected type {}", nzsl::Ast::ToString(varType));
		throw std::runtime_error("unexpected type " + nzsl::Ast::ToString(varType));
	}
} // namespace cct::gfx
