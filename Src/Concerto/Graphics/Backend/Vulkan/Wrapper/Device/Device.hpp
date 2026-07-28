//
// Created by arthur on 25/10/2022.
//

#ifndef CONCERTO_GRAPHICS_DEVICE_HPP
#define CONCERTO_GRAPHICS_DEVICE_HPP

#include <memory>
#include <span>
#include <unordered_map>
#include <unordered_set>

#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Allocator/Allocator.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Queue/Queue.hpp"

namespace cct::gfx::vk
{
	class PhysicalDevice;
	class Instance;
	class UploadContext;
#ifdef CCT_ENABLE_OBJECT_DEBUG
	class ObjectDebug;
#endif

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API Device : public Object<VkDevice>
	{
	public:
		Device() = default;
		explicit Device(PhysicalDevice& physicalDevice);
		~Device() override;

		Device(Device&&) noexcept;
		Device(const Device&) = delete;

		Device& operator=(Device&&) noexcept;
		Device& operator=(const Device&) = delete;

		VkResult Create(PhysicalDevice& physicalDevice);

		[[nodiscard]] UInt32 GetQueueFamilyIndex(Queue::Type queueType) const;
		[[nodiscard]] UInt32 GetQueueFamilyIndex(UInt32 queueFlag) const;
		[[nodiscard]] Queue& GetQueue(Queue::Type queueType);

		void WaitIdle() const;

		void UpdateDescriptorSetsWrite(std::span<VkWriteDescriptorSet> descriptorWrites) const;
		void UpdateDescriptorSetWrite(const VkWriteDescriptorSet& descriptorWrite) const;

		PhysicalDevice& GetPhysicalDevice() const;
		Allocator& GetAllocator() const;
		Instance& GetInstance() const;

		bool IsExtensionEnabled(const std::string& ext) const; // fixme: use std::string_view

		[[nodiscard]] bool IsVideoDecodeSupported() const
		{
			return m_videoDecodeSupported;
		}
		[[nodiscard]] UInt32 GetVideoDecodeQueueFamilyIndex() const
		{
			return m_videoDecodeQueueFamily;
		}

		[[nodiscard]] const std::unordered_set<std::string>& GetEnabledExtensions() const
		{
			return m_extensions;
		}

#define CONCERTO_VULKAN_BACKEND_DEVICE_FUNCTION(func) PFN_##func func = nullptr;
#define CONCERTO_VULKAN_BACKEND_DEVICE_EXT_FUNCTION(func, ...) CONCERTO_VULKAN_BACKEND_DEVICE_FUNCTION(func)

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/DeviceFunction.hpp"
	private:
		void CreateAllocator();

		PhysicalDevice* m_physicalDevice;
		std::unique_ptr<Allocator> m_allocator;
		std::unordered_map<Queue::Type, Queue> m_queues;
		std::unordered_set<std::string> m_extensions;
		bool m_videoDecodeSupported = false;
		UInt32 m_videoDecodeQueueFamily = 0xFFFFFFFFU;
	};

} // namespace cct::gfx::vk

#endif // CONCERTO_GRAPHICS_DEVICE_HPP
