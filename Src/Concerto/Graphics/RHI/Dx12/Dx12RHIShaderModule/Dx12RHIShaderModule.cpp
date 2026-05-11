//
// Created by arthur on 23/10/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIShaderModule/Dx12RHIShaderModule.hpp"

#include <Concerto/Core/Assert.hpp>
#if 0
#include <NZSL/DxilWriter.hpp>
#endif
namespace cct::gfx::rhi
{
	namespace
	{
		nzsl::ShaderStageType ToNzslStage(cct::gfx::ShaderStage stage)
		{
			switch (stage)
			{
				case ShaderStage::Vertex:
					return nzsl::ShaderStageType::Vertex;
				case ShaderStage::Fragment:
					return nzsl::ShaderStageType::Fragment;
				case ShaderStage::Compute:
					return nzsl::ShaderStageType::Compute;
				default:
					break;
			}
			CCT_ASSERT_FALSE("ConcertoGraphics: Unexpected shader stage");
			return nzsl::ShaderStageType::Vertex;
		}
	} // namespace

	Dx12RHIShaderModule::Dx12RHIShaderModule(cct::gfx::ResolvedShaderModule&& resolved) :
		m_bindings(std::move(resolved.bindings)),
		m_entryPointName(std::move(resolved.entryPointName)),
		m_stage(resolved.stage)
	{
#if 0
		nzsl::DxilWriter dxilWriter;
		nzsl::DxilWriter::Environment env;
		env.shaderModelMajorVersion = 6;
		env.shaderModelMinorVersion = 8;
		dxilWriter.SetEnv(env);

		auto output = dxilWriter.Generate(ToNzslStage(m_stage), *resolved.resolvedAst);
		m_compiledBytecode.resize(output.dxil.size());
		std::memcpy(m_compiledBytecode.data(), output.dxil.data(), output.dxil.size());
#endif
	}

	Dx12RHIShaderModule::~Dx12RHIShaderModule() = default;

	const std::vector<UInt32>& Dx12RHIShaderModule::GetShaderBytes() const
	{
		return m_emptyShaderBytes;
	}

	const std::unordered_map<UInt32, std::vector<cct::gfx::DescriptorSetLayoutBinding>>& Dx12RHIShaderModule::GetBindings() const
	{
		return m_bindings;
	}

	const std::string& Dx12RHIShaderModule::GetEntryPointName() const
	{
		return m_entryPointName;
	}

	cct::gfx::ShaderStage Dx12RHIShaderModule::GetStage() const
	{
		return m_stage;
	}

	D3D12_SHADER_BYTECODE Dx12RHIShaderModule::GetD3D12ShaderBytecode() const
	{
		D3D12_SHADER_BYTECODE bytecode = {};
		bytecode.pShaderBytecode = m_compiledBytecode.data();
		bytecode.BytecodeLength = m_compiledBytecode.size();
		return bytecode;
	}
} // namespace cct::gfx::rhi
