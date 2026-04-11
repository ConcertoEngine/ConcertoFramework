//
// Created by arthur on 11/06/22.
//

#ifndef CONCERTO_GRAPHICS_SWAPCHAIN_HPP
#define CONCERTO_GRAPHICS_SWAPCHAIN_HPP

#include <vector>
#include <optional>
#include <span>


#include "Concerto/Graphics/Backend/Vulkan/Defines.hpp"

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Image/Image.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/ImageView/ImageView.hpp"

namespace cct::gfx
{
	class Window;
}

namespace cct::gfx::vk
{
	class Device;
	class PhysicalDevice;
	class RenderPass;
	class Fence;
	class Semaphore;

	class CONCERTO_GRAPHICS_VULKAN_BACKEND_API SwapChain : public Object<VkSwapchainKHR>
	{
	public:
		SwapChain(Device& device, Window& window, VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB, VkFormat depthFormat = VK_FORMAT_D32_SFLOAT);
		~SwapChain() override;

		SwapChain(SwapChain&&) noexcept = default;
		SwapChain(const SwapChain&) = delete;

		SwapChain& operator=(SwapChain&&) noexcept = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		VkResult Create(Device& device, Window& window, VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB, VkFormat depthFormat = VK_FORMAT_D32_SFLOAT);
		void Destroy();

		[[nodiscard]] std::span<Image> GetImages() const;
		[[nodiscard]] std::span<ImageView> GetImageViews() const;
		[[nodiscard]] VkExtent2D GetExtent() const;
		[[nodiscard]] const ImageView& GetDepthImageView() const;
		[[nodiscard]] ImageView& GetDepthImageView();
		[[nodiscard]] VkFormat GetImageFormat() const;
		[[nodiscard]] VkFormat GetDepthFormat() const;
		[[nodiscard]] UInt32 GetCurrentImageIndex() const;

		VkResult AcquireNextImage(const Semaphore& semaphore, UInt32& imageIndex, const Fence* fence = nullptr, UInt64 timeout = std::numeric_limits<UInt64>::max());

		Window& GetWindow() const;
	private:
		VkResult CreateSurface();

		mutable std::optional<std::vector<Image>> m_swapChainImages;
		mutable std::optional<std::vector<ImageView>> m_swapChainImageViews;
		VkExtent2D m_windowExtent;
		VkFormat m_swapChainImageFormat;
		Image m_depthImage;
		ImageView m_depthImageView;
		Window* m_window;
		UInt32 m_currentImageIndex;
		VkSurfaceKHR m_surface;
	};
} // cct::gfx::vk

#endif //CONCERTO_GRAPHICS_SWAPCHAIN_HPP
