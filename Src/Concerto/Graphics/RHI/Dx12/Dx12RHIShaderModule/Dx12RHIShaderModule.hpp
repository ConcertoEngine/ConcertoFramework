//
// Created by arthur on 23/10/2025.
//

#ifndef CONCERTO_GRAPHICS_RHI_DX12_DX12RHISHADERMODULE_HPP
#define CONCERTO_GRAPHICS_RHI_DX12_DX12RHISHADERMODULE_HPP

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <d3d12.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "Concerto/Graphics/Core/ShaderModule/ShaderModule.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/ShaderModule.hpp"

namespace cct::gfx::rhi
{
	class CONCERTO_GRAPHICS_RHI_BASE_API Dx12RHIShaderModule : public ShaderModule
	{
	public:
		Dx12RHIShaderModule() = delete;
		explicit Dx12RHIShaderModule(cct::gfx::ShaderModule&& shaderModule);
		~Dx12RHIShaderModule();
		Dx12RHIShaderModule(const Dx12RHIShaderModule&) = delete;
		Dx12RHIShaderModule(Dx12RHIShaderModule&&) = default;

		Dx12RHIShaderModule& operator=(const Dx12RHIShaderModule&) = delete;
		Dx12RHIShaderModule& operator=(Dx12RHIShaderModule&&) = default;

		// RHI ShaderModule interface implementation
		const std::vector<UInt32>& GetShaderBytes() const override;
		const std::unordered_map<UInt32, std::vector<cct::gfx::DescriptorSetLayoutBinding>>& GetBindings() const override;
		const std::string& GetEntryPointName() const override;
		cct::gfx::ShaderStage GetStage() const override;

		// DX12-specific methods
		D3D12_SHADER_BYTECODE GetD3D12ShaderBytecode() const;

	private:
		void CompileToDxil();

		cct::gfx::ShaderModule m_abstractShaderModule;
		std::vector<Byte> m_compiledBytecode; // DXIL bytecode
		std::vector<UInt32> m_emptyShaderBytes; // empty, for interface compat (DX12 has no SPIR-V to expose)
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_DX12_DX12RHISHADERMODULE_HPP
