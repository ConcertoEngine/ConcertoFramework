//
// Created by arthur on 03/11/2022.
//

#ifndef CONCERTO_GRAPHICS_OBJECT_INL
#define CONCERTO_GRAPHICS_OBJECT_INL

#include <utility>

#include <Concerto/Core/Assert.hpp>
#include <Concerto/Core/TypeInfo/TypeInfo.hpp>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	template<typename VkType>
	Object<VkType>::Object() :
		ObjectDebug(),
		m_handle(nullptr),
		m_device(nullptr),
		m_lastResult(VK_SUCCESS)
	{
	}

	template<typename VkType>
	Object<VkType>::Object(Device& device) :
#ifdef CCT_ENABLE_OBJECT_DEBUG
		ObjectDebug(device, TypeName<std::remove_pointer_t<std::remove_cvref_t<VkType>>>(), reinterpret_cast<void**>(&this->m_handle)),
#else
		ObjectDebug(),
#endif // CCT_ENABLE_OBJECT_DEBUG
		m_handle(nullptr),
		m_device(&device),
		m_lastResult(VK_SUCCESS)
	{
	}

	template<typename VkType>
	Object<VkType>::~Object()
	{
	}

	template<typename VkType>
	Object<VkType>::Object(Object&& other) noexcept :
		ObjectDebug(std::move(other))
	{
		m_handle = std::exchange(other.m_handle, nullptr);
		m_device = std::exchange(other.m_device, nullptr);
		m_lastResult = std::exchange(other.m_lastResult, VK_SUCCESS);
	}

	template<typename VkType>
	Object<VkType>& Object<VkType>::operator=(Object&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_device, other.m_device);

		ObjectDebug::operator=(std::move(other));

		return *this;
	}

	template<typename VkType>
	VkType* Object<VkType>::Get() const
	{
		CCT_ASSERT(IsValid(), "The vulkan object handle is null");
		return const_cast<VkType*>(&m_handle);
	}

	template<typename VkType>
	bool Object<VkType>::IsValid() const
	{
		return m_handle != nullptr;
	}

	template<typename VkType>
	Device* Object<VkType>::GetDevice() const
		requires(!std::is_same_v<VkType, VkDevice> && !std::is_same_v<
														  VkType, VkInstance>)
	{
		CCT_ASSERT(m_device, "Invalid device");
		return m_device;
	}

	template<typename VkType>
	VkResult Object<VkType>::GetLastResult() const
	{
		return m_lastResult;
	}
} // namespace cct::gfx::vk

#endif // CONCERTO_GRAPHICS_OBJECT_INL
