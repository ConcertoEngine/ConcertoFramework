//
// Created by arthur on 11/06/22.
//


#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/Core/Window/Window.hpp"

#include "Concerto/Graphics/Backend/Vulkan/VkException.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/SwapChain/SwapChain.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Fence/Fence.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice/PhysicalDevice.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/RenderPass/RenderPass.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Semaphore/Semaphore.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance/Instance.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/FrameBuffer/FrameBuffer.hpp"

namespace cct::gfx::vk
{
	SwapChain::SwapChain(Device& device, Window& window, VkFormat colorFormat, VkFormat depthFormat) : Object(device),
		m_swapChainImages(),
		m_swapChainImageViews(),
		m_windowExtent(),
		m_swapChainImageFormat(),
		m_depthImage(),
		m_depthImageView(),
		m_window(&window),
		m_currentImageIndex(0),
		m_surface(VK_NULL_HANDLE)
	{
		if (Create(device, window, colorFormat, depthFormat) != VK_SUCCESS)
			throw VkException(GetLastResult());
	}

	SwapChain::~SwapChain()
	{
		if (!IsValid())
			return;
		Destroy();
	}

	VkResult SwapChain::Create(Device& device, Window& window, VkFormat colorFormat, VkFormat depthFormat)
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();

		Destroy();

		m_device = &device;
		m_windowExtent = { .width = window.GetWidth(), .height = window.GetHeight() },
		m_depthImage = device.GetAllocator().AllocateImage(m_windowExtent, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT),

		m_lastResult = m_depthImageView.Create(device, m_depthImage, VK_IMAGE_ASPECT_DEPTH_BIT);
		if (m_lastResult != VK_SUCCESS)
			return m_lastResult;

		m_window = &window,
		m_windowExtent = { .width = m_window->GetWidth(), .height = m_window->GetHeight() };

		m_lastResult = CreateSurface();
		if (m_lastResult != VK_SUCCESS)
			return m_lastResult;

		PhysicalDevice::SurfaceSupportDetails surfaceSupportDetails = m_device->GetPhysicalDevice().GetSurfaceSupportDetails(m_surface);

		// Select the best format actually supported by the surface
		VkFormat selectedFormat = VK_FORMAT_UNDEFINED;
		for (const VkSurfaceFormatKHR& surfaceFormat : surfaceSupportDetails.formats)
		{
			if (surfaceFormat.format == colorFormat)
			{
				selectedFormat = colorFormat;
				break;
			}
		}
		if (selectedFormat == VK_FORMAT_UNDEFINED && !surfaceSupportDetails.formats.empty())
			selectedFormat = surfaceSupportDetails.formats[0].format;
		else if (selectedFormat == VK_FORMAT_UNDEFINED)
			selectedFormat = colorFormat;
		m_swapChainImageFormat = selectedFormat;

		VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
		UInt32 imageCount = surfaceSupportDetails.capabilities.minImageCount + 1;
		if (surfaceSupportDetails.capabilities.maxImageCount > 0 && imageCount > surfaceSupportDetails.capabilities.maxImageCount)
			imageCount = surfaceSupportDetails.capabilities.maxImageCount;

		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.surface = m_surface;
		swapChainCreateInfo.minImageCount = imageCount;
		swapChainCreateInfo.imageFormat = m_swapChainImageFormat;
		swapChainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		swapChainCreateInfo.imageExtent = m_windowExtent;
		swapChainCreateInfo.imageArrayLayers = 1;
		swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCreateInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		swapChainCreateInfo.clipped = VK_TRUE;
		swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		m_lastResult = m_device->vkCreateSwapchainKHR(*m_device->Get(), &swapChainCreateInfo, nullptr, &m_handle);
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateSemaphore failed VkResult={}", static_cast<const int>(m_lastResult));
		return m_lastResult;
	}

	void SwapChain::Destroy()
	{
		if (m_handle)
		{
			m_device->vkDestroySwapchainKHR(*m_device->Get(), m_handle, nullptr);
			m_handle = nullptr;
		}

		if (m_surface)
		{
			m_device->GetInstance().vkDestroySurfaceKHR(*GetDevice()->GetInstance().Get(), m_surface, nullptr);
			m_surface = nullptr;
		}

		m_swapChainImages = std::nullopt;
		m_swapChainImageViews = std::nullopt;
		m_windowExtent = {};
		m_swapChainImageFormat = {};
		m_depthImage = {};
		m_depthImageView = {};
		m_window = nullptr;
		m_currentImageIndex = {};
		m_surface = nullptr;
	}

	std::span<Image> SwapChain::GetImages() const
	{
		if (m_swapChainImages.has_value())
			return m_swapChainImages.value();
		UInt32 imageCount;
		std::vector<VkImage> swapChainImages;
		m_device->vkGetSwapchainImagesKHR(*m_device->Get(), m_handle, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		m_device->vkGetSwapchainImagesKHR(*m_device->Get(), m_handle, &imageCount, swapChainImages.data());
		std::vector<Image> images;
		images.reserve(imageCount);
		for (auto& image: swapChainImages)
			images.emplace_back(m_device->GetAllocator(), m_windowExtent, image, m_swapChainImageFormat);
		m_swapChainImages = std::move(images);
		return m_swapChainImages.value();
	}

	std::span<ImageView> SwapChain::GetImageViews() const
	{
		if (m_swapChainImageViews)
			return m_swapChainImageViews.value();
		std::vector<ImageView> swapChainImageViews;
		swapChainImageViews.reserve(GetImages().size());
		for (auto& image: GetImages())
			swapChainImageViews.emplace_back(*m_device, image, VK_IMAGE_ASPECT_COLOR_BIT);
		m_swapChainImageViews = std::move(swapChainImageViews);
		return m_swapChainImageViews.value();
	}

	VkExtent2D SwapChain::GetExtent() const
	{
		return m_windowExtent;
	}

	const ImageView& SwapChain::GetDepthImageView() const
	{
		return m_depthImageView;
	}

	ImageView& SwapChain::GetDepthImageView()
	{
		return m_depthImageView;
	}

	VkFormat SwapChain::GetImageFormat() const
	{

		return m_swapChainImageFormat;
	}

	VkFormat SwapChain::GetDepthFormat() const
	{
		return m_depthImage.GetFormat();
	}

	UInt32 SwapChain::GetCurrentImageIndex() const
	{
		return m_currentImageIndex;
	}

	VkResult SwapChain::AcquireNextImage(const Semaphore& semaphore, UInt32& imageIndex, const Fence* fence, UInt64 timeout)
	{
		m_lastResult = m_device->vkAcquireNextImageKHR(*m_device->Get(), m_handle, timeout, *semaphore.Get(), fence ? *fence->Get() : nullptr, &m_currentImageIndex);
		imageIndex = m_currentImageIndex;
		if (m_lastResult == VK_ERROR_OUT_OF_DATE_KHR || m_lastResult == VK_SUBOPTIMAL_KHR)
			return m_lastResult;
		else if (m_lastResult != VK_SUCCESS)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: vkCreateSemaphore failed VKResult={}", static_cast<int>(m_lastResult));
			return m_lastResult;
		}
		return VK_SUCCESS;
	}

	Window& SwapChain::GetWindow() const
	{
		return *m_window;
	}

	VkResult SwapChain::CreateSurface()
	{
		NativeWindow nativeWindow = m_window->GetNativeWindow();
#if defined(CCT_PLATFORM_WINDOWS)
		const VkWin32SurfaceCreateInfoKHR createInfo = {
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.hinstance = static_cast<HINSTANCE>(nativeWindow.hinstance),
			.hwnd = static_cast<HWND>(nativeWindow.window)
		};
		m_lastResult = m_device->GetInstance().vkCreateWin32SurfaceKHR(*m_device->GetInstance().Get(), &createInfo, nullptr, &m_surface);
#elif defined(CCT_PLATFORM_MACOS)
		CCT_ASSERT_FALSE("Not implemented");
#elif defined(CCT_PLATFORM_LINUX)
		if (std::holds_alternative<NativeWindow::X11>(nativeWindow.platform))
		{
			auto& x11 = std::get<NativeWindow::X11>(nativeWindow.platform);
			const VkXlibSurfaceCreateInfoKHR createInfo = {
				.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
				.pNext = nullptr,
				.flags = 0,
				.dpy = static_cast<Display*>(x11.display),
				.window = static_cast<::Window>(x11.window)
			};
			m_lastResult = m_device->GetInstance().vkCreateXlibSurfaceKHR(*m_device->GetInstance().Get(), &createInfo, nullptr, &m_surface);
		}
		else if (std::holds_alternative<NativeWindow::Wayland>(nativeWindow.platform))
		{
			auto& wayland = std::get<NativeWindow::Wayland>(nativeWindow.platform);
			const VkWaylandSurfaceCreateInfoKHR createInfo = {
				.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
				.pNext = nullptr,
				.flags = 0,
				.display = static_cast<struct wl_display*>(wayland.wl_display),
				.surface = static_cast<struct wl_surface*>(wayland.wl_surface)
			};
			m_lastResult = m_device->GetInstance().vkCreateWaylandSurfaceKHR(*m_device->GetInstance().Get(), &createInfo, nullptr, &m_surface);
		}
		else
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: Unsupported Linux windowing system");
			return VK_ERROR_INITIALIZATION_FAILED;
		}
#endif
		CCT_ASSERT(m_lastResult == VK_SUCCESS, "ConcertoGraphics: vkCreateWin32SurfaceKHR failed VKResult={}", static_cast<int>(m_lastResult));

		return m_lastResult;
	}
}
