//
// Created by arthur on 10/06/22.
//

#include <cassert>
#include <fstream>
#include <iostream>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ShaderModule/ShaderModule.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{

	ShaderModule::ShaderModule(Device& device, const std::string& shaderPath, VkShaderStageFlagBits stageFlags, const std::string& entryPoint /*= "main"*/) :
		Object(device),
		m_stageFlags(stageFlags),
		m_entryPoint(entryPoint)
	{
		if (Create(device, shaderPath, stageFlags, entryPoint) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	ShaderModule::ShaderModule(Device& device, const std::vector<UInt32>& bytes, VkShaderStageFlagBits stageFlags, const std::string& entryPoint /*= "main"*/) :
		Object(device),
		m_stageFlags(stageFlags),
		m_entryPoint(entryPoint)
	{
		if (Create(device, bytes, stageFlags, entryPoint) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	ShaderModule::~ShaderModule()
	{
		if (!IsValid())
			return;
		m_device->vkDestroyShaderModule(*m_device->Get(), m_handle, nullptr);
	}

	VkResult ShaderModule::Create(Device& device, const std::string& shaderPath, VkShaderStageFlagBits stageFlags, const std::string& entryPoint)
	{
		m_device = &device;
		m_stageFlags = stageFlags;
		m_entryPoint = entryPoint;

		LoadShaderModule(shaderPath);
		return CreateShaderModule();
	}

	VkResult ShaderModule::Create(Device& device, const std::vector<UInt32>& bytes, VkShaderStageFlagBits stageFlags, const std::string& entryPoint)
	{
		m_device = &device;
		m_stageFlags = stageFlags;
		m_entryPoint = entryPoint;

		m_shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		m_shaderModuleCreateInfo.pNext = nullptr;
		m_shaderModuleCreateInfo.codeSize = bytes.size() * sizeof(UInt32);
		m_shaderModuleCreateInfo.pCode = bytes.data();

		return CreateShaderModule();
	}

	VkPipelineShaderStageCreateInfo ShaderModule::GetPipelineShaderStageCreateInfo() const
	{
		return VkPipelineShaderStageCreateInfo {
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			{},
			m_stageFlags,
			m_handle,
			m_entryPoint.c_str(),
			nullptr
		};
	}

	void ShaderModule::LoadShaderModule(const std::string& shaderPath)
	{
		std::ifstream file(shaderPath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Failed to open shader module '{}'", shaderPath);
			return;
		}
		std::streamsize fileSize = file.tellg();
		m_buffer.resize(fileSize / sizeof(UInt32));
		file.seekg(0);
		file.read(reinterpret_cast<char*>(m_buffer.data()), fileSize);
		file.close();
		m_shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		m_shaderModuleCreateInfo.pNext = nullptr;
		m_shaderModuleCreateInfo.codeSize = m_buffer.size() * sizeof(UInt32);
		m_shaderModuleCreateInfo.pCode = m_buffer.data();
	}

	VkResult ShaderModule::CreateShaderModule()
	{
		m_lastResult = m_device->vkCreateShaderModule(*m_device->Get(), &m_shaderModuleCreateInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateShaderModule failed VKResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}
}