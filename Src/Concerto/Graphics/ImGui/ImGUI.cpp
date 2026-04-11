//
// Created by arthu on 21/12/2022.
//

#include "Concerto/Graphics/ImGUI.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Instance.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/PhysicalDevice.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/Device.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/RenderPass.hpp"
#include "Concerto/Graphics/Backend/Vulkan/Wrapper/CommandBuffer.hpp"
#include "Concerto/Graphics/Window/Window.hpp"

namespace cct::gfx::vk
{
	ImGUI::ImGUI(RenderingContext& context, Window& window) : m_context(context),
																	   m_descriptorPool(*m_context.device,
																			   {{ VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
																				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
																				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
																				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
																				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
																				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
																				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
																				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
																				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
																				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
																				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }}),
																	   m_window(window)
	{

		IMGUI_CHECKVERSION();
		m_imGuiContext = ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = *m_context.instance->Get();
		init_info.PhysicalDevice = *m_context.physicalDevice->Get();
		init_info.Device = *m_context.device->Get();
		init_info.QueueFamily = m_context.queueFamilyIndex;
		init_info.Queue = *m_context.queue->Get();
		init_info.DescriptorPool = *m_descriptorPool.Get();
		init_info.MinImageCount = m_context.minImageCount;
		init_info.ImageCount = m_context.imageCount;
		init_info.MSAASamples = m_context.MSAASamples;
		init_info.RenderPass = *m_context.renderPass->Get();

		ImGui_ImplVulkan_Init(&init_info);
		const bool res = ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)window.GetRawWindow(), true);
		if (res == false)
		{
			CCT_ASSERT_FALSE("ConcertoGraphics: error ImGui_ImplGlfw_InitForVulkan");
			return;
		}
		m_context.commandBuffer->ImmediateSubmit(*m_context.fence, *m_context.commandPool, *m_context.queue,
				[](CommandBuffer&)
				{
					const bool res = ImGui_ImplVulkan_CreateFontsTexture();
					if (res == false)
					{
						CCT_ASSERT_FALSE("ConcertoGraphics: error ImGui_ImplVulkan_CreateFontsTexture");
					}
				});
	}

	ImGUI::~ImGUI()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGUI::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGUI::Draw()
	{
		ImGui::Render();
	}

	void ImGUI::RenderDrawData(CommandBuffer& commandBuffer)
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer.Get());
	}

	ImGuiContext* ImGUI::GetContext() const
	{
		return m_imGuiContext;
	}

	void ImGUI::UpdateMousePosition(float x, float y)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMousePosEvent(x, y);
	}

	void ImGUI::UpdateMouseButton(int button, bool pressed)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent(button, pressed);
	}
} // cct::gfx