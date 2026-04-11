//
// Created by arthur on 25/10/2022.
//

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <vector>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Core/Logger/Logger.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"
#define VOLK_IMPLEMENTATION
#include <volk.h> // must be under this ^ include

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/cpptrace.hpp>
namespace cct::gfx::vk
{
	PFN_vkGetInstanceProcAddr Instance::vkGetInstanceProcAddr = nullptr;

	namespace
	{
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
			[[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			[[maybe_unused]] void* pUserData)
		{
			if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			{
				CCT_VK_LOG_DEBUG("{}", pCallbackData->pMessage);
			}
			else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			{
				CCT_VK_LOG_INFO("{}", pCallbackData->pMessage);
			}
			else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				CCT_VK_LOG_WARN("{}", pCallbackData->pMessage);
			}
			else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				CCT_VK_LOG_ERROR("{}\n Trace: \n{}", pCallbackData->pMessage, cpptrace::generate_trace().to_string());
			}
			return VK_FALSE;
		}
	}

	Instance::Instance(const std::string& appName, const std::string& engineName, const Version& apiVersion,
		const Version& appVersion, const Version& engineVersion, std::span<const char*> extensions,
		std::span<const char*> layers) :
		m_apiVersion(apiVersion)
	{
		if (Create(appName, engineName, apiVersion, appVersion, engineVersion, extensions, layers) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	Instance::~Instance()
	{
		if (m_debugMessenger != VK_NULL_HANDLE && vkDestroyDebugUtilsMessengerEXT)
			vkDestroyDebugUtilsMessengerEXT(m_handle, m_debugMessenger, nullptr);
		vkDestroyInstance(m_handle, nullptr);
	}

	VkResult Instance::Create(const std::string& appName, const std::string& engineName, const Version& apiVersion,
		const Version& appVersion, const Version& engineVersion, std::span<const char*> extensions,
		std::span<const char*> layers)
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();
		m_apiVersion = apiVersion;

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = appName.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(appVersion.major, appVersion.minor, appVersion.patch);
		appInfo.pEngineName = engineName.c_str();
		appInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
		appInfo.apiVersion = VK_MAKE_VERSION(apiVersion.major, apiVersion.minor, apiVersion.patch);

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;
		debugCreateInfo.pUserData = nullptr;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<UInt32>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.empty() ? VK_NULL_HANDLE : extensions.data();
		createInfo.enabledLayerCount = static_cast<UInt32>(layers.size());
		createInfo.ppEnabledLayerNames = layers.empty() ? VK_NULL_HANDLE : layers.data();
		createInfo.pNext = &debugCreateInfo;

		//std::array enables = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
		//VkValidationFeaturesEXT features = {};
		//features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		//features.enabledValidationFeatureCount = enables.size();
		//features.pEnabledValidationFeatures = enables.data();

		//if (createInfo.pNext)
		//{
		//	features.pNext = createInfo.pNext;
		//	createInfo.pNext = &features;
		//}
		//else
		//{
		//	createInfo.pNext = &features;
		//}

		for (auto& ext : extensions)
			m_loadedExtensions.emplace(ext);
		for (auto& layer : layers)
			m_loadedLayers.emplace(layer);

		m_lastResult = volkInitialize();
		if (m_lastResult != VK_SUCCESS)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: volkInitialize() failed VkResult={}", static_cast<int>(m_lastResult));
			throw std::runtime_error("volkInitialize Failed");
		}

		m_lastResult = vkCreateInstance(&createInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateInstance failed VKResult={}", static_cast<int>(m_lastResult));
		volkLoadInstanceOnly(m_handle);
		Instance::vkGetInstanceProcAddr = ::vkGetInstanceProcAddr;
#define CONCERTO_VULKAN_BACKEND_INSTANCE_FUNCTION(func) this->func = ::func;

#define CONCERTO_VULKAN_BACKEND_INSTANCE_EXT_BEGIN(ext)				\
					if(IsExtensionEnabled(#ext))							\
					{
#define CONCERTO_VULKAN_BACKEND_INSTANCE_EXT_FUNCTION(func, ...)	\
						CONCERTO_VULKAN_BACKEND_INSTANCE_FUNCTION(func)		\
						if (this->func == nullptr)							\
						{													\
							CCT_ASSERT_FALSE("ConcertoGraphics: Function: " #func " is null but the extension has been reported has supported");\
						}
#define CONCERTO_VULKAN_BACKEND_INSTANCE_EXT_END }

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/InstanceFunction.hpp"

		if (IsExtensionEnabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) && vkCreateDebugUtilsMessengerEXT)
		{
			m_lastResult = vkCreateDebugUtilsMessengerEXT(m_handle, &debugCreateInfo, nullptr, &m_debugMessenger);
			CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateDebugUtilsMessengerEXT failed VkResult={}", static_cast<int>(m_lastResult));
		}

		return m_lastResult;
	}

	Version Instance::GetApiVersion() const
	{
		return m_apiVersion;
	}

	std::span<PhysicalDevice> Instance::EnumeratePhysicalDevices() const
	{
		if (m_physicalDevices)
			return m_physicalDevices.value();
		UInt32 deviceCount = 0;
		m_lastResult = vkEnumeratePhysicalDevices(m_handle, &deviceCount, nullptr);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkEnumeratePhysicalDevices failed VKResult={}", static_cast<int>(m_lastResult));
		std::vector<VkPhysicalDevice> devices(deviceCount);
		m_lastResult = vkEnumeratePhysicalDevices(m_handle, &deviceCount, devices.data());
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkEnumeratePhysicalDevices failed VKResult={}", static_cast<int>(m_lastResult));
		std::vector<PhysicalDevice> physicalDevices;
		physicalDevices.reserve(devices.size());
		for (VkPhysicalDevice device : devices)
		{
			physicalDevices.emplace_back(const_cast<Instance&>(*this), device);
		}
		m_physicalDevices = std::move(physicalDevices);
		return m_physicalDevices.value();
	}

	bool Instance::IsExtensionEnabled(const std::string& ext) const
	{
		return m_loadedExtensions.contains(ext);
	}

	void Instance::SetLogger(Logger& logger)
	{
		Logger::SetContext(&logger);
	}
} // namespace cct::gfx::vk
