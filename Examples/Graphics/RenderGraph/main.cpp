//
// Created by arthur on 12/07/2022.
//

#include <algorithm>
#include <chrono>
#include <span>

#include <Concerto/Core/Math/Algorithm.hpp>
#include <Concerto/Graphics/Core/Camera/Camera.hpp>
#include <Concerto/Graphics/Core/Camera/GPUData.hpp>
#include <Concerto/Graphics/Core/DisplayManager/DisplayManager.hpp>
#include <Concerto/Graphics/Core/Window/Window.hpp>
#include <Concerto/Graphics/Renderer/ForwardOpaqueFeature.hpp>
#include <Concerto/Graphics/Renderer/Renderer.hpp>
#include <Concerto/Graphics/Renderer/RenderScene.hpp>
#include <Concerto/Graphics/Renderer/RenderTarget.hpp>
#include <Concerto/Graphics/Renderer/View.hpp>
#include <Concerto/Graphics/RHI/CommandBuffer.hpp>
#include <Concerto/Graphics/RHI/Frame.hpp>
#include <Concerto/Graphics/RHI/GpuMesh.hpp>
#include <Concerto/Graphics/RHI/Instance/APIImpl.hpp>
#include <Concerto/Graphics/RHI/Instance/Instance.hpp>
#include <Concerto/Graphics/RHI/MaterialBuilder.hpp>
#include <Concerto/Graphics/RHI/Mesh/Mesh.hpp>
#include <Concerto/Graphics/RHI/RenderPass.hpp>
#include <Concerto/Graphics/RHI/SwapChain.hpp>
#include <Concerto/Graphics/RHI/Texture.hpp>

using namespace cct;
using namespace cct::gfx;

int main()
{
	try
	{
		Logger logger;
		Logger::SetContext(&logger);
		rhi::Instance::SetLogger(logger);

		DisplayManager displayManager;
		auto window = displayManager.CreateWindow(1, "Concerto Graphics - RenderGraph", 1280, 720);
		Input& inputManager = window->GetInputManager();
		rhi::Instance rInstance(rhi::Instance::Backend::Vulkan, rhi::ValidationFlags::Standard | rhi::ValidationFlags::SyncValidation);
		std::unique_ptr<rhi::Device> device;

		std::size_t deviceIndex = 0;
		for (const auto& [name, vendor, type] : rInstance.EnumerateDevices())
		{
			if (type == rhi::DeviceType::Dedicated)
			{
				Logger::Info("Using device: {}", name);
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

		RenderScene scene;
		Renderer renderer(*device, swapChain->GetImageCount());
		renderer.AddFeature(std::make_unique<ForwardOpaqueFeature>(scene));

		std::unique_ptr<rhi::TextureBuilder> textureBuilder = device->CreateTextureBuilder();
		std::unique_ptr<rhi::MaterialBuilder> materialBuilder = device->CreateMaterialBuilder(swapChain->GetExtent(), *textureBuilder);

		const Vector2u extent = swapChain->GetExtent();
		std::vector<bool> backbufferEverUsed(swapChain->GetImageCount(), false);

		RenderTarget target;
		target.colorTexture = swapChain->GetColorTexture(0);
		target.currentLayout = rhi::ImageLayout::Undefined;
		target.exportLayout = rhi::ImageLayout::PresentSrcKhr;
		target.colorFormat = swapChain->GetPixelFormat();
		target.depthFormat = swapChain->GetDepthPixelFormat();
		target.width = extent.X();
		target.height = extent.Y();
		renderer.Build(target);

		std::shared_ptr<rhi::GpuMesh> gpuMesh = device->CreateMesh(
			"./assets/sponza/sponza.obj", *materialBuilder, *textureBuilder, renderer.GetPassRenderPass("SceneOpaque"));

		constexpr int gridSize = 10;
		constexpr float gridSpacing = 3000.f;
		for (int x = 0; x < gridSize; ++x)
		{
			for (int z = 0; z < gridSize; ++z)
			{
				const float offsetX = static_cast<float>(x - gridSize / 2) * gridSpacing;
				const float offsetZ = static_cast<float>(z - gridSize / 2) * gridSpacing;
				scene.AddInstance({.mesh = gpuMesh, .transform = Vector3f(offsetX, 0.f, offsetZ).ToTranslationMatrix()});
			}
		}

		float aspect = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
		Camera camera(ToRadians(90.f), 0.1f, 1000000.f, aspect);
		bool cursorDisabled = false;
		float deltaTime = 0.f;
		float speed = 15000.f;
		window->SetCursorDisabled(cursorDisabled);

		window->RegisterResizeCallback(camera, [&](Window& window)
									   {
			aspect = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
			camera.SetAspectRatio(aspect);
			camera.SetFov(45.f);
			camera.SetNear(0.0001f);
			camera.SetFar(1000.f); });
		inputManager.Register("MouseMoved", MouseEvent::Type::Moved, camera, [&camera](const MouseEvent& e)
							  { camera.Rotate(e.mouseMove.deltaX, -e.mouseMove.deltaY); });

		inputManager.Register("Forward", Key::Z, TriggerType::Pressed, camera, [&camera, &speed, &deltaTime]()
							  { camera.Move(Camera::CameraMovement::Forward, deltaTime * speed); });

		inputManager.Register("Backward", Key::S, TriggerType::Pressed, camera, [&camera, &speed, &deltaTime]()
							  { camera.Move(Camera::CameraMovement::Backward, deltaTime * speed); });

		inputManager.Register("Left", Key::Q, TriggerType::Pressed, camera, [&camera, &speed, &deltaTime]()
							  { camera.Move(Camera::CameraMovement::Left, deltaTime * speed); });

		inputManager.Register("Right", Key::D, TriggerType::Pressed, camera, [&camera, &speed, &deltaTime]()
							  { camera.Move(Camera::CameraMovement::Right, deltaTime * speed); });

		inputManager.Register("MouseFocused", Key::LeftAlt, TriggerType::Pressed, *window, [&cursorDisabled, &window]()
							  {
			cursorDisabled = !cursorDisabled;
			window->SetCursorDisabled(cursorDisabled); });

		Scene sceneParameters = {};
		sceneParameters.gpuSceneData.sunlightDirection = Vector4f{3.1f, 1.f, -1.f, 0};
		sceneParameters.gpuSceneData.ambientColor = Vector4f{0.f, 0.f, 0.f, 1.f};
		sceneParameters.gpuSceneData.sunlightColor = Vector4f{255.f, 109.f, 39.f, 1.f};
		sceneParameters.clearColor = Vector4f{0.1f, 0.1f, 0.1f, 1.f};

		std::unique_ptr<rhi::Buffer> cameraBuffer = device->CreateBuffer(static_cast<rhi::BufferUsageFlags>(rhi::BufferUsage::Uniform), sizeof(GPUCamera), true);
		std::unique_ptr<rhi::Buffer> sceneBuffer = device->CreateBuffer(static_cast<rhi::BufferUsageFlags>(rhi::BufferUsage::Uniform), sizeof(Scene), true);
		std::unique_ptr<rhi::Buffer> objectsBuffer = device->CreateBuffer(
			static_cast<rhi::BufferUsageFlags>(rhi::BufferUsage::Storage),
			static_cast<UInt32>(sizeof(GPUObjectData) * scene.GetInstances().size()),
			true);

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

			GPUCamera gpuCamera = camera.ToGPUCamera();
			cameraBuffer->Write(gpuCamera, rhi::PadUniformBuffer(sizeof(GPUCamera), minimumAlignment * currentFrame.GetCurrentFrameIndex()));
			sceneBuffer->Write(sceneParameters.gpuSceneData);
			objectsBuffer->Write<GPUObjectData, const MeshInstance>(
				std::span<const MeshInstance>(scene.GetInstances()),
				[](GPUObjectData& dest, const MeshInstance& src)
				{ dest.modelMatrix = src.transform; });

			const auto imageIndex = static_cast<UInt32>(currentFrame.GetCurrentFrameIndex());
			const auto width = static_cast<UInt32>(window->GetWidth());
			const auto height = static_cast<UInt32>(window->GetHeight());
			const View view = View::FromCamera(camera, Vector2u{width, height});

			if (width != target.width || height != target.height)
				std::fill(backbufferEverUsed.begin(), backbufferEverUsed.end(), false);

			target.colorTexture = swapChain->GetColorTexture(imageIndex);
			target.currentLayout = backbufferEverUsed[imageIndex] ? rhi::ImageLayout::PresentSrcKhr : rhi::ImageLayout::Undefined;
			target.width = width;
			target.height = height;
			backbufferEverUsed[imageIndex] = true;

			rhi::CommandBuffer& commandBuffer = currentFrame.GetCommandBuffer();
			commandBuffer.Reset();
			commandBuffer.Begin();
			{
				renderer.DrawFrame(commandBuffer, target, view);
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
