//
// Created by arthur on 12/07/2022.
//

#pragma optimize("", off)

#include <chrono>

#include <Concerto/Core/Math/Algorithm.hpp>
#include <Concerto/Graphics/Core/Window/Window.hpp>
#include <Concerto/Graphics/RHI/CommandBuffer.hpp>
#include <Concerto/Graphics/RHI/Frame.hpp>
#include <Concerto/Graphics/RHI/FrameBuffer.hpp>
#include <Concerto/Graphics/RHI/Instance/APIImpl.hpp>
#include <Concerto/Graphics/RHI/Instance/Instance.hpp>
#include <Concerto/Graphics/RHI/MaterialBuilder.hpp>
#include <Concerto/Graphics/RHI/Mesh/Mesh.hpp>
#include <Concerto/Graphics/RHI/RenderPass.hpp>
#include <Concerto/Graphics/RHI/SwapChain.hpp>
#include <Concerto/Graphics/RHI/Texture.hpp>

#include "Concerto/Graphics/Core/Camera/Camera.hpp"
#include "Concerto/Graphics/Core/DisplayManager/DisplayManager.hpp"
#include "Concerto/Graphics/RHI/GpuMesh.hpp"
#include "Concerto/Graphics/RHI/GpuSubMesh/GpuSubMesh.hpp"

using namespace cct;
using namespace cct::gfx;

int main()
{
	try
	{
		Logger logger;
		Logger::SetContext(&logger);

		DisplayManager displayManager;
		auto displayInfos = displayManager.EnumerateDisplaysInfos();

		Logger::Info("Displays:");
		for (auto& displayInfo : displayInfos)
		{
			Logger::Info("\t{} primary: {}", displayInfo.displayName, displayInfo.isPrimary);
			Logger::Info("\t\tDisplay modes:");
			for (auto& displayMode : displayInfo.displayModes)
			{
				Logger::Info("\t\t\twidth:{}, height:{}", displayMode.width, displayMode.height);
			}
		}
		auto window = displayManager.CreateWindow(1, "Concerto Graphics", 1280, 720);
		Input& inputManager = window->GetInputManager();

		rhi::Instance rInstance(rhi::Instance::Backend::Vulkan);
		std::unique_ptr<rhi::Device> device;

		std::size_t deviceIndex = 0;
		for (const auto& [name, vendor, type] : rInstance.EnumerateDevices())
		{
			if (type == rhi::DeviceType::Dedicated)
			{
				cct::Logger::Info("Using device: {}", name);
				device = rInstance.CreateDevice(deviceIndex);
				break;
			}
			++deviceIndex;
		}

		if (device == nullptr)
			device = rInstance.CreateDevice(0);

		if (!device)
		{
			CCT_ASSERT_FALSE("Could not find a device");
			return EXIT_FAILURE;
		}
		std::size_t minimumAlignment = device->GetMinimumUniformBufferOffsetAlignment();
		std::unique_ptr<rhi::SwapChain> swapChain = device->CreateSwapChain(*window);
		rhi::RenderPass* renderPass = swapChain->GetRenderPass();

		std::unique_ptr<rhi::MaterialBuilder> materialBuilder = device->CreateMaterialBuilder(swapChain->GetExtent());
		std::unique_ptr<rhi::TextureBuilder> textureBuilder = device->CreateTextureBuilder();
		std::shared_ptr<rhi::GpuMesh> gpuMesh = device->CreateMesh("./assets/sponza/sponza.obj", *materialBuilder, *renderPass);

		// GraphicPass& pbrPass = graphBuilder.AddPass("pbr", rhi::PipelineStage::AllGraphics);
		// pbrPass.AddColorOutput("albedo", ??);
		// pbrPass.AddColorOutput("normal", ??);

		float aspect = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
		Camera camera(ToRadians(90.f), 0.1f, 1000000.f, aspect);
		bool cursorDisabled = false;
		float deltaTime = 0.f;
		float speed = 15000.f;
		window->SetCursorDisabled(cursorDisabled);
		window->RegisterResizeCallback([&](Window& window)
									   {
			aspect = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
			camera.SetAspectRatio(aspect);
			camera.SetFov(45.f);
			camera.SetNear(0.0001f);
			camera.SetFar(1000.f); });
		inputManager.Register("MouseMoved", MouseEvent::Type::Moved, [&camera](const MouseEvent& e)
							  { camera.Rotate(e.mouseMove.deltaX, -e.mouseMove.deltaY); });

		inputManager.Register("Forward", Key::Z, TriggerType::Pressed, [&camera, &speed, &deltaTime]()
							  { camera.Move(Camera::CameraMovement::Forward, deltaTime * speed); });

		inputManager.Register("Backward", Key::S, TriggerType::Pressed, [&camera, &speed, &deltaTime]()
							  { camera.Move(Camera::CameraMovement::Backward, deltaTime * speed); });

		inputManager.Register("Left", Key::Q, TriggerType::Pressed, [&camera, &speed, &deltaTime]()
							  { camera.Move(Camera::CameraMovement::Left, deltaTime * speed); });

		inputManager.Register("Right", Key::D, TriggerType::Pressed, [&camera, &speed, &deltaTime]()
							  { camera.Move(Camera::CameraMovement::Right, deltaTime * speed); });

		inputManager.Register("MouseFocused", Key::LeftAlt, TriggerType::Pressed, [&cursorDisabled, &window]()
							  {
			cursorDisabled = !cursorDisabled;
			window->SetCursorDisabled(cursorDisabled); });

		Scene sceneParameters = {};
		sceneParameters.gpuSceneData.sunlightDirection = Vector4f{3.1f, 1.f, -1.f, 0};
		sceneParameters.gpuSceneData.ambientColor = Vector4f{0.f, 0.f, 0.f, 1.f};
		sceneParameters.gpuSceneData.sunlightColor = Vector4f{255.f, 109.f, 39.f, 1.f};
		sceneParameters.clearColor = Vector4f{0.1f, 0.1f, 0.1f, 1.f};

		const Vector3f position(0.f, 0.f, 0.f);
		const EulerAnglesf rotation(0, 0, 0);
		const Vector3f scale(1.f, 1.f, 1.f);
		auto modelMatrix = Matrix4f::Identity();
		modelMatrix *= position.ToTranslationMatrix();
		modelMatrix *= rotation.ToQuaternion().ToRotationMatrix<Matrix4f>();
		modelMatrix *= scale.ToScalingMatrix();

		std::unique_ptr<rhi::Buffer> cameraBuffer = device->CreateBuffer(static_cast<rhi::BufferUsageFlags>(rhi::BufferUsage::Uniform), sizeof(GPUCamera), true);
		std::unique_ptr<rhi::Buffer> sceneBuffer = device->CreateBuffer(static_cast<rhi::BufferUsageFlags>(rhi::BufferUsage::Uniform), sizeof(Scene), true);
		std::unique_ptr<rhi::Buffer> objectsBuffer = device->CreateBuffer(static_cast<rhi::BufferUsageFlags>(rhi::BufferUsage::Storage), sizeof(GPUObjectData), true);

		materialBuilder->Update(*cameraBuffer, 0, 0);
		materialBuilder->Update(*sceneBuffer, 0, 1);
		materialBuilder->Update(*objectsBuffer, 1, 0);

		std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
		while (!window->ShouldClose())
		{
			displayManager.DispatchEvents();
			camera.UpdateViewProjectionMatrix();
			auto beginTime = std::chrono::high_resolution_clock::now();
			deltaTime = std::chrono::duration<float>(beginTime - lastFrameTime).count();
			lastFrameTime = beginTime;
			rhi::Frame& currentFrame = swapChain->AcquireFrame();
			rhi::CommandBuffer& commandBuffer = currentFrame.GetCommandBuffer();
			cameraBuffer->Write<GPUCamera>(camera, rhi::PadUniformBuffer(sizeof(GPUCamera), minimumAlignment * currentFrame.GetCurrentFrameIndex()));
			sceneBuffer->Write(sceneParameters.gpuSceneData);
			objectsBuffer->Write(modelMatrix);
			renderPass = swapChain->GetRenderPass();
			commandBuffer.Reset();
			commandBuffer.Begin();
			{
				Viewport viewport{
					.x = 0.f,
					.y = 0.f,
					.width = static_cast<float>(window->GetWidth()),
					.height = static_cast<float>(window->GetHeight()),
					.minDepth = 0.f,
					.maxDepth = 1.f,
				};
				Rect2D dynamicScissor = {};
				dynamicScissor.x = 0;
				dynamicScissor.y = 0;
				dynamicScissor.width = window->GetWidth();
				dynamicScissor.height = window->GetHeight();
				commandBuffer.SetViewport(viewport);
				commandBuffer.SetScissor(dynamicScissor);
				commandBuffer.BeginRenderPass(*renderPass, currentFrame.GetFrameBuffer(), Vector3f{1.f, 0.f, 0.f});
				{
					std::size_t lastBoundMaterial = 0;
					for (const auto& subMesh : gpuMesh->subMeshes)
					{
						const auto& material = subMesh->GetMaterial();
						if (material == nullptr)
							continue;
						std::size_t materialHash = material->GetHash();
						if (lastBoundMaterial != materialHash)
						{
							lastBoundMaterial = materialHash;
							commandBuffer.BindMaterial(*material);
						}
						commandBuffer.BindVertexBuffer(subMesh->GetVertexBuffer());
						commandBuffer.Draw(static_cast<UInt32>(subMesh->GetVertices().size()), 1, 0, 0);
					}
				}
				commandBuffer.EndRenderPass();
			}
			commandBuffer.End();
			currentFrame.Present();
			window->SetTitle(std::format("ConcertoGraphics - {} fps", 1.f / deltaTime));
		}

		swapChain->WaitAll();
		device->WaitIdle();
	}
	catch (const std::exception& e)
	{
		Logger::Error("An unhandled exception was thrown: '{}'", e.what());
	}
	return 0;
}
