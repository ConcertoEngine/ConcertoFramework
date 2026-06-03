//
// Created by arthur on 01/09/2025.
//

#ifndef CONCERTO_GRAPHICS_BACKEND_D3D12_SWAPCHAIN_HPP
#define CONCERTO_GRAPHICS_BACKEND_D3D12_SWAPCHAIN_HPP

#include <array>

#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/Backend/Dx12/Wrapper/DescriptorHeap/DescriptorHeap.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Object/Object.hpp"
#include "Concerto/Graphics/Backend/Dx12/Wrapper/Queue/Queue.hpp"
#include "Concerto/Graphics/Core/PixelFormat.hpp"
#include "Concerto/Graphics/Core/Window/Window.hpp"

namespace cct::gfx::dx12
{
	class CONCERTO_GRAPHICS_DX12_BACKEND_API SwapChain : public Object<IDXGISwapChain3>
	{
	public:
		static constexpr UInt32 ImageCount = 3;

		SwapChain() = default;
		SwapChain(Device& device, Window& window, PixelFormat pixelFormat = PixelFormat::BGRA8_SRGB);
		~SwapChain() override = default;

		SwapChain(const SwapChain&) = delete;
		SwapChain(SwapChain&&) = default;

		SwapChain& operator=(const SwapChain&) = delete;
		SwapChain& operator=(SwapChain&&) = default;

		HRESULT Create(Device& device, Window& window, PixelFormat pixelFormat);

		Queue& GetCommandQueue();
		Vector2u GetExtent() const;

		[[nodiscard]] const std::array<D3D12_CPU_DESCRIPTOR_HANDLE, ImageCount>& GetRenderTargetViewHandles() const
		{
			return m_renderTargetViewHandles;
		}
		[[nodiscard]] const std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, ImageCount>& GetRenderTargets() const
		{
			return m_renderTargets;
		}

	private:
		Window* m_window;
		Queue m_directCommandQueue;
		DescriptorHeap m_renderTargetViewHeap;
		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, ImageCount> m_renderTargetViewHandles;
		std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, ImageCount> m_renderTargets;
	};
} // namespace cct::gfx::dx12

#endif // CONCERTO_GRAPHICS_BACKEND_D3D12_SWAPCHAIN_HPP
