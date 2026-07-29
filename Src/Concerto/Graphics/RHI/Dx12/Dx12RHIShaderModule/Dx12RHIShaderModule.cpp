//
// Created by arthur on 23/10/2025.
//

#include "Concerto/Graphics/RHI/Dx12/Dx12RHIShaderModule/Dx12RHIShaderModule.hpp"

#include <cstring>
#include <dxcapi.h>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Core/Defines.hpp"
#include <spirv_cross/spirv_hlsl.hpp>
#include <wrl/client.h>

namespace cct::gfx::rhi
{
	namespace
	{
		const wchar_t* ToHlslTarget(cct::gfx::ShaderStage stage)
		{
			switch (stage)
			{
				case ShaderStage::Vertex:
					return L"vs_6_0";
				case ShaderStage::Fragment:
					return L"ps_6_0";
				case ShaderStage::Compute:
					return L"cs_6_0";
				default:
					break;
			}
			CCT_ASSERT_FALSE("ConcertoGraphics: Unexpected shader stage");
			return L"vs_6_0";
		}

		spv::ExecutionModel ToSpvExecutionModel(cct::gfx::ShaderStage stage)
		{
			switch (stage)
			{
				case ShaderStage::Vertex:
					return spv::ExecutionModelVertex;
				case ShaderStage::Fragment:
					return spv::ExecutionModelFragment;
				case ShaderStage::Compute:
					return spv::ExecutionModelGLCompute;
				default:
					break;
			}
			CCT_ASSERT_FALSE("ConcertoGraphics: Unexpected shader stage");
			return spv::ExecutionModelVertex;
		}
	} // namespace

	Dx12RHIShaderModule::Dx12RHIShaderModule(cct::gfx::ShaderModule&& shaderModule) :
		m_abstractShaderModule(std::move(shaderModule))
	{
		CompileToDxil();
	}

	Dx12RHIShaderModule::~Dx12RHIShaderModule() = default;

	void Dx12RHIShaderModule::CompileToDxil()
	{
		const std::vector<UInt32>& spirv = m_abstractShaderModule.GetShaderBytes();
		if (spirv.empty())
			return;

		const std::string& entryPointName = m_abstractShaderModule.GetEntryPointName();
		const cct::gfx::ShaderStage stage = m_abstractShaderModule.GetStage();
		const spv::ExecutionModel executionModel = ToSpvExecutionModel(stage);

		std::string hlslSource;
		std::string hlslEntryPoint;
		try
		{
			spirv_cross::CompilerHLSL compiler(spirv);

			for (const auto& entryPoint : compiler.get_entry_points_and_stages())
			{
				if (entryPoint.name == entryPointName && entryPoint.execution_model == executionModel)
				{
					compiler.set_entry_point(entryPoint.name, entryPoint.execution_model);
					break;
				}
			}

			spirv_cross::CompilerHLSL::Options hlslOptions;
			hlslOptions.shader_model = 60;
			compiler.set_hlsl_options(hlslOptions);

			hlslSource = compiler.compile();
			hlslEntryPoint = compiler.get_cleansed_entry_point_name(entryPointName, executionModel);
		}
		catch (const std::exception& e)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: SPIRV-Cross failed to generate HLSL for '{}': {}", entryPointName, e.what());
			return;
		}

		// 2. HLSL source -> DXIL bytecode (DXC)
		Microsoft::WRL::ComPtr<IDxcUtils> utils;
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
		if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))) ||
			FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler))))
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Failed to create DXC compiler instances");
			return;
		}

		Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
		utils->CreateBlob(hlslSource.data(), static_cast<UINT32>(hlslSource.size()), DXC_CP_UTF8, &sourceBlob);

		const std::wstring entryPointW(hlslEntryPoint.begin(), hlslEntryPoint.end());
		const wchar_t* target = ToHlslTarget(stage);

		std::vector<LPCWSTR> args = {
			L"-E",
			entryPointW.c_str(),
			L"-T",
			target,
		};

		DxcBuffer sourceBuffer{};
		sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
		sourceBuffer.Size = sourceBlob->GetBufferSize();
		sourceBuffer.Encoding = DXC_CP_UTF8;

		Microsoft::WRL::ComPtr<IDxcResult> result;
		HRESULT hr = dxcCompiler->Compile(&sourceBuffer, args.data(), static_cast<UINT32>(args.size()), nullptr, IID_PPV_ARGS(&result));
		if (FAILED(hr) || !result)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: DXC::Compile call failed for '{}'", entryPointName);
			return;
		}

		Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors;
		result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		if (errors && errors->GetStringLength() > 0)
			CCT_GFX_LOG_WARN("Dx12RHIShaderModule", "DXC ({}): {}", entryPointName, errors->GetStringPointer());

		HRESULT compileStatus = S_OK;
		result->GetStatus(&compileStatus);
		if (FAILED(compileStatus))
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: DXC failed to compile '{}' to DXIL", entryPointName);
			return;
		}

		Microsoft::WRL::ComPtr<IDxcBlob> dxil;
		result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&dxil), nullptr);
		if (!dxil)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: DXC produced no DXIL output for '{}'", entryPointName);
			return;
		}

		m_compiledBytecode.resize(dxil->GetBufferSize());
		std::memcpy(m_compiledBytecode.data(), dxil->GetBufferPointer(), dxil->GetBufferSize());
	}

	const std::vector<UInt32>& Dx12RHIShaderModule::GetShaderBytes() const
	{
		return m_emptyShaderBytes;
	}

	const std::unordered_map<UInt32, std::vector<cct::gfx::DescriptorSetLayoutBinding>>& Dx12RHIShaderModule::GetBindings() const
	{
		return m_abstractShaderModule.GetBindings();
	}

	const std::string& Dx12RHIShaderModule::GetEntryPointName() const
	{
		return m_abstractShaderModule.GetEntryPointName();
	}

	cct::gfx::ShaderStage Dx12RHIShaderModule::GetStage() const
	{
		return m_abstractShaderModule.GetStage();
	}

	D3D12_SHADER_BYTECODE Dx12RHIShaderModule::GetD3D12ShaderBytecode() const
	{
		D3D12_SHADER_BYTECODE bytecode = {};
		bytecode.pShaderBytecode = m_compiledBytecode.data();
		bytecode.BytecodeLength = m_compiledBytecode.size();
		return bytecode;
	}
} // namespace cct::gfx::rhi
