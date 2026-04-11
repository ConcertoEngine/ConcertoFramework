//
// Created by arthur on 24/10/2022.
//

#include <cassert>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Sampler/Sampler.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/VulkanInitializer/VulkanInitializer.hpp"

namespace cct::gfx::vk
{
	Sampler::Sampler(Device& device, VkFilter filter, VkSamplerAddressMode samplerAddressMode) : Object(device)
	{
		if (Create(device, filter, samplerAddressMode) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	Sampler::~Sampler()
	{
		if (!IsValid())
			return;
		m_device->vkDestroySampler(*m_device->Get(), m_handle, nullptr);
	}

	VkResult Sampler::Create(Device& device, VkFilter filter, VkSamplerAddressMode samplerAddressMode)
	{
		m_device = &device;

		VkSamplerCreateInfo samplerInfo = VulkanInitializer::SamplerCreateInfo(filter, samplerAddressMode);

		m_lastResult = m_device->vkCreateSampler(*m_device->Get(), &samplerInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateSampler failed VkResult={}", static_cast<const int>(m_lastResult));

		return m_lastResult;
	}
}
