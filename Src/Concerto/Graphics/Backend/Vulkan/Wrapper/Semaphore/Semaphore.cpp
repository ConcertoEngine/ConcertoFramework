//
// Created by arthur on 15/06/22.
//

#include <stdexcept>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Semaphore/Semaphore.hpp"

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{

	Semaphore::Semaphore(Device& device) : Object(device)
	{
		if (Create(device) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	Semaphore::~Semaphore()
	{
		if (!IsValid())
			return;
		m_device->vkDestroySemaphore(*m_device->Get(), m_handle, nullptr);
	}

	VkResult Semaphore::Create(Device& device)
	{
		m_device = &device;

		VkSemaphoreCreateInfo info = {
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			info.pNext = nullptr,
			info.flags = 0
		};

		m_lastResult = m_device->vkCreateSemaphore(*m_device->Get(), &info, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateSemaphore failed VkResult={}", static_cast<const int>(m_lastResult));

		return m_lastResult;
	}

	Result<UInt64, VkResult> Semaphore::GetSemaphoreCounterValue() const
	{
		CCT_ASSERT(IsValid(), "Invalid object state, 'Create' must be called");

		UInt64 value;
		VkResult result = m_device->vkGetSemaphoreCounterValue(*m_device->Get(), *Get(), & value);
		if (result == VK_SUCCESS)
			return value;
		return result;
	}
} // namespace cct::gfx::vk
