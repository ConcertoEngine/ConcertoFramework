//
// Created by arthur on 21/12/2022.
//

#ifndef CONCERTO_GRAPHICS_IMGUI_HPP
#define CONCERTO_GRAPHICS_IMGUI_HPP

#include <imgui.h>
#include "Concerto/Graphics/Core/Defines.hpp"

#include "Concerto/Graphics/Backend/Vulkan/RenderingContext.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/DescriptorPool.hpp"

namespace cct::gfx
{
	class Window;
}

namespace cct::gfx::vk
{
	class CommandBuffer;

	class CONCERTO_GRAPHICS_CORE_API ImGUI
	{
	public:
		explicit ImGUI(RenderingContext &context, Window &window);
		~ImGUI();

		/**
		 * @brief Begin a new Dear ImGui frame
		 */
		void NewFrame();

		/**
		 * @brief Render the Dear ImGui frame
		 * @info This function must be called after the rendering pass
		 * @param commandBuffer The command buffer to record the rendering commands
		 */
		void RenderDrawData(CommandBuffer &commandBuffer);

		/**
		 * @brief End the Dear ImGui frame, finalize the draw data
		 */
		void Draw();

		/**
		 * @brief Get the Dear ImGui context
		 * @return The Dear ImGui context
		 */
		[[nodiscard]] ImGuiContext* GetContext() const;

		/**
		 * @bief Queue a mouse position update
		 * @param x The x position
		 * @param y The y position
		 */
		void UpdateMousePosition(float x, float y);

		/**
		 * @bief Queue a mouse button update
		 * @param button The button index
		 * @param pressed The button state
		 */
		void UpdateMouseButton(int button, bool pressed);

	private:
		RenderingContext& m_context;
		DescriptorPool m_descriptorPool;
		Window& m_window;
		ImGuiContext* m_imGuiContext;
	};

} // cct::gfx

#endif //CONCERTO_GRAPHICS_IMGUI_HPP
