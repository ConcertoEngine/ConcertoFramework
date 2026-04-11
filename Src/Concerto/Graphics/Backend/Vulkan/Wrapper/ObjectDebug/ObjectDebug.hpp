//
// Created by arthur on 25/10/2023.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_VULKAN_OBJECTDEBUG_HPP
#define CONCERTO_GRAPHICS_BACKEND_VULKAN_OBJECTDEBUG_HPP

#include <string>
#include <string_view>
#include <thread>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"

namespace cct::gfx::vk
{
	class Device;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API ObjectDebug
	{
	public:
#ifdef CCT_ENABLE_OBJECT_DEBUG
		ObjectDebug();
		ObjectDebug(Device& device, std::string_view typeName, void** vkHandle);
		virtual ~ObjectDebug() = default;

		VkDebugReportObjectTypeEXT GetDebugReportObjectType() const;
		UInt64 GetObject() const;

		void SetDebugName(std::string_view name);
		std::string_view GetDebugName() const;
		std::thread::id GetCreatedOnThread() const;

		ObjectDebug(ObjectDebug&& other) noexcept;
		ObjectDebug(const ObjectDebug& other) = delete;

		ObjectDebug& operator=(ObjectDebug&& other) noexcept;
		ObjectDebug& operator=(const ObjectDebug& other) = delete;

	private:
		Device* m_device;
		std::string_view m_typeName;
		VkDebugReportObjectTypeEXT m_debugReportObjectType;
		void** m_vkHandle;
		std::string m_debugName;
		std::thread::id m_createdOnThread;
#else
		ObjectDebug() = default;
		virtual ~ObjectDebug() = default;

		ObjectDebug(ObjectDebug&& other) noexcept = default;
		ObjectDebug(const ObjectDebug& other) = delete;

		ObjectDebug& operator=(ObjectDebug&& other) noexcept = default;
		ObjectDebug& operator=(const ObjectDebug& other) = delete;
#endif // CCT_ENABLE_OBJECT_DEBUG
	};

}

#endif //CONCERTO_GRAPHICS_BACKEND_VULKAN_OBJECTDEBUG_HPP