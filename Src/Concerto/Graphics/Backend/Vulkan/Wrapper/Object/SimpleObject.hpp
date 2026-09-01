//
// Created by arthur on 11/09/2026.
//

#ifndef CONCERTO_GRAPHICS_SIMPLEOBJECT_HPP
#define CONCERTO_GRAPHICS_SIMPLEOBJECT_HPP

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"

namespace cct::gfx::vk
{
	class Device;

	template<typename VkType, auto DestroyFn>
	class SimpleObject : public Object<VkType>
	{
	public:
		SimpleObject() = default;
		explicit SimpleObject(Device& device);
		~SimpleObject() override;

		SimpleObject(SimpleObject&&) noexcept = default;
		SimpleObject(const SimpleObject&) = delete;

		SimpleObject& operator=(SimpleObject&&) noexcept = default;
		SimpleObject& operator=(const SimpleObject&) = delete;
	};
} // namespace cct::gfx::vk

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/SimpleObject.inl"

#endif // CONCERTO_GRAPHICS_SIMPLEOBJECT_HPP
