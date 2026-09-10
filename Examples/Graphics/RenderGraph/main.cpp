//
// Created by arthur on 12/07/2022.
//

#include <algorithm>
#include <chrono>

#include <Concerto/Core/Math/Algorithm.hpp>
#include <Concerto/Graphics/Core/Camera/Camera.hpp>
#include <Concerto/Graphics/Core/Camera/GPUData.hpp>
#include <Concerto/Graphics/Core/DisplayManager/DisplayManager.hpp>
#include <Concerto/Graphics/Core/Window/Window.hpp>
#include <Concerto/Graphics/RenderGraph/RenderGraph.hpp>
#include <Concerto/Graphics/RHI/CommandBuffer.hpp>
#include <Concerto/Graphics/RHI/Frame.hpp>
#include <Concerto/Graphics/RHI/GpuMesh.hpp>
#include <Concerto/Graphics/RHI/GpuSubMesh/GpuSubMesh.hpp>
#include <Concerto/Graphics/RHI/Instance/APIImpl.hpp>
#include <Concerto/Graphics/RHI/Instance/Instance.hpp>
#include <Concerto/Graphics/RHI/MaterialBuilder.hpp>
#include <Concerto/Graphics/RHI/Mesh/Mesh.hpp>
#include <Concerto/Graphics/RHI/RenderPass.hpp>
#include <Concerto/Graphics/RHI/SwapChain.hpp>
#include <Concerto/Graphics/RHI/Texture.hpp>

using namespace cct;
using namespace cct::gfx;

namespace
{
	void DrawScene(rhi::RenderGraphContext& ctx, const rhi::GpuMesh& gpuMesh)
	{
		rhi::CommandBuffer& cmd = ctx.GetCommandBuffer();
		cmd.SetViewport({
			.x = 0.f,
			.y = 0.f,
			.width = static_cast<float>(ctx.GetWidth()),
			.height = static_cast<float>(ctx.GetHeight()),
			.minDepth = 0.f,
			.maxDepth = 1.f,
		});
		cmd.SetScissor({0, 0, ctx.GetWidth(), ctx.GetHeight()});

		std::size_t lastBoundMaterial = 0;
		for (const auto& subMesh : gpuMesh.subMeshes)
		{
			const auto& material = subMesh->GetMaterial();
			if (material == nullptr)
				continue;
			const std::size_t materialHash = material->GetHash();
			if (lastBoundMaterial != materialHash)
			{
				lastBoundMaterial = materialHash;
				cmd.BindMaterial(*material);
			}
			cmd.BindVertexBuffer(subMesh->GetVertexBuffer());
			cmd.Draw(static_cast<UInt32>(subMesh->GetVertices().size()), 1, 0, 0);
		}
	}
} // namespace

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

		rhi::RenderGraph renderGraph(*device, swapChain->GetImageCount());

		std::unique_ptr<rhi::TextureBuilder> textureBuilder = device->CreateTextureBuilder();
		std::unique_ptr<rhi::MaterialBuilder> materialBuilder = device->CreateMaterialBuilder(swapChain->GetExtent(), *textureBuilder);

		std::shared_ptr<rhi::GpuMesh> gpuMesh;
		rhi::RGTextureHandle backbufferHandle;
		rhi::RGTextureHandle depthHandle;
		UInt32 declaredWidth = 0;
		UInt32 declaredHeight = 0;

		auto declareGraph = [&](UInt32 width, UInt32 height, UInt32 imageIndex, rhi::ImageLayout backbufferLayout)
		{
			renderGraph.Clear();

			backbufferHandle = renderGraph.ImportTexture(
				"Backbuffer", swapChain->GetColorTexture(imageIndex), backbufferLayout,
				swapChain->GetPixelFormat(), width, height);
			depthHandle = renderGraph.CreateTexture(
				{width, height, swapChain->GetDepthPixelFormat(), true, "SceneDepth"});

			renderGraph.AddGraphicsPass(
				"SceneOpaque",
				[&](rhi::RenderGraphBuilder& b)
				{
					backbufferHandle = b.Write(backbufferHandle);
					depthHandle = b.WriteDepth(depthHandle);
				},
				[&](rhi::RenderGraphContext& ctx)
				{ DrawScene(ctx, *gpuMesh); });

			renderGraph.SetFinalOutput(backbufferHandle);
			renderGraph.SetExportLayout(backbufferHandle, rhi::ImageLayout::PresentSrcKhr);

			declaredWidth = width;
			declaredHeight = height;
		};

		const Vector2u extent = swapChain->GetExtent();
		declareGraph(extent.X(), extent.Y(), 0, rhi::ImageLayout::Undefined);
		renderGraph.Compile();

		const rhi::RenderPass& sceneRenderPass = renderGraph.GetPassRenderPass("SceneOpaque");
		gpuMesh = device->CreateMesh("./assets/sponza/sponza.obj", *materialBuilder, *textureBuilder, sceneRenderPass);

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

		std::vector<bool> backbufferEverUsed(swapChain->GetImageCount(), false);

		std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
		while (!window->ShouldClose())
		{
			displayManager.DispatchEvents();
			camera.UpdateViewProjectionMatrix();
			auto beginTime = std::chrono::high_resolution_clock::now();
			deltaTime = std::chrono::duration<float>(beginTime - lastFrameTime).count();
			lastFrameTime = beginTime;

			rhi::Frame& currentFrame = swapChain->AcquireFrame();

			renderGraph.Reset();

			cameraBuffer->Write<GPUCamera>(camera, rhi::PadUniformBuffer(sizeof(GPUCamera), minimumAlignment * currentFrame.GetCurrentFrameIndex()));
			sceneBuffer->Write(sceneParameters.gpuSceneData);
			objectsBuffer->Write(modelMatrix);

			const auto imageIndex = static_cast<UInt32>(currentFrame.GetCurrentFrameIndex());
			const auto width = static_cast<UInt32>(window->GetWidth());
			const auto height = static_cast<UInt32>(window->GetHeight());

			if (width != declaredWidth || height != declaredHeight)
			{
				std::fill(backbufferEverUsed.begin(), backbufferEverUsed.end(), false);
				declareGraph(width, height, imageIndex, rhi::ImageLayout::Undefined);
				backbufferEverUsed[imageIndex] = true;
			}
			else
			{
				const rhi::ImageLayout backbufferLayout = backbufferEverUsed[imageIndex]
															  ? rhi::ImageLayout::PresentSrcKhr
															  : rhi::ImageLayout::Undefined;
				backbufferEverUsed[imageIndex] = true;
				renderGraph.UpdateImportedTexture(backbufferHandle, swapChain->GetColorTexture(imageIndex), backbufferLayout);
			}

			rhi::CommandBuffer& commandBuffer = currentFrame.GetCommandBuffer();
			commandBuffer.Reset();
			commandBuffer.Begin();
			{
				renderGraph.Execute(commandBuffer, width, height);
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
