//
// Created by arthur on 03/11/2022.
//

#ifndef CONCERTO_GRAPHICS_OBJECT_HPP
#define CONCERTO_GRAPHICS_OBJECT_HPP

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ObjectDebug/ObjectDebug.hpp"

namespace cct::gfx::vk
{
	class Device;

	template<typename VkType>
	class Object : public ObjectDebug
	{
	public:
		Object();
		explicit Object(Device& device);
		~Object() override;

		Object(const Object&) = delete;
		Object(Object&&) noexcept;

		Object& operator=(const Object&) = delete;
		Object& operator=(Object&&) noexcept;

		[[nodiscard]] VkType* Get() const;
		[[nodiscard]] bool IsValid() const;
		[[nodiscard]] Device* GetDevice() const requires (!std::is_same_v<VkType, VkDevice> && !std::is_same_v<VkType, VkInstance>);
		[[nodiscard]] VkResult GetLastResult() const;

	protected:
		VkType m_handle;
		Device* m_device;
		mutable VkResult m_lastResult;
	};
}

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.inl"

#endif //CONCERTO_GRAPHICS_OBJECT_HPP
