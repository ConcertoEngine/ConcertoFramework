//
// Created by arthur on 11/09/2026.
//

#ifndef CONCERTO_GRAPHICS_SIMPLEOBJECT_INL
#define CONCERTO_GRAPHICS_SIMPLEOBJECT_INL

// DestroyFn is a pointer to a Device member, so the destructor below needs Device to be a complete type.
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"

namespace cct::gfx::vk
{
	template<typename VkType, auto DestroyFn>
	SimpleObject<VkType, DestroyFn>::SimpleObject(Device& device) :
		Object<VkType>(device)
	{
	}

	template<typename VkType, auto DestroyFn>
	SimpleObject<VkType, DestroyFn>::~SimpleObject()
	{
		if (!this->IsValid())
			return;
		(this->m_device->*DestroyFn)(*this->m_device->Get(), this->m_handle, nullptr);
	}
} // namespace cct::gfx::vk

#endif // CONCERTO_GRAPHICS_SIMPLEOBJECT_INL
