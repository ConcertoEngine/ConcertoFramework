//
// Created by arthur on 11/05/2026.
//
// Demonstrates the RenderGraph API:
//   - transient texture creation (GBufferAlbedo, GBufferDepth)
//   - external resource import (persistent off-screen target)
//   - multi-pass setup with automatic barrier generation
//   - pass culling (CulledPass writes to an unread texture)
//   - DumpGraph() debug output
//   - per-frame Execute / Reset cycle
//

#include <chrono>
#include <format>
#include <iostream>

#include <Concerto/Graphics/Core/DisplayManager/DisplayManager.hpp>
#include <Concerto/Graphics/Core/Window/Window.hpp>
#include <Concerto/Graphics/RenderGraph/RenderGraph.hpp>
#include <Concerto/Graphics/RHI/CommandBuffer.hpp>
#include <Concerto/Graphics/RHI/Frame.hpp>
#include <Concerto/Graphics/RHI/Instance/APIImpl.hpp>
#include <Concerto/Graphics/RHI/Instance/Instance.hpp>
#include <Concerto/Graphics/RHI/SwapChain.hpp>

using namespace cct;
using namespace cct::gfx;

int main()
{
	try
	{
		Logger logger;
		Logger::SetContext(&logger);

		DisplayManager displayManager;
		auto window = displayManager.CreateWindow(1, "RenderGraph Example", 1280, 720);

		rhi::Instance rInstance(rhi::Instance::Backend::Vulkan);
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
		if (!device)
			device = rInstance.CreateDevice(0);
		if (!device)
		{
			Logger::Error("No GPU device found");
			return EXIT_FAILURE;
		}

		std::unique_ptr<rhi::SwapChain> swapChain = device->CreateSwapChain(*window);
		const UInt32 W = window->GetWidth();
		const UInt32 H = window->GetHeight();

		// ------------------------------------------------------------------
		// Persistent off-screen texture — owned outside the graph, imported
		// each frame so the graph can write to it.
		// ------------------------------------------------------------------
		std::shared_ptr<rhi::Texture> offscreenColor =
			device->CreateTexture(PixelFormat::RGBA8_SRGB, static_cast<Int32>(W), static_cast<Int32>(H));

		// ------------------------------------------------------------------
		// Render graph setup (done once; call MarkDirty() + Compile() again
		// on window resize).
		// ------------------------------------------------------------------
		rhi::RenderGraph rg(*device, swapChain->GetImageCount());

		// Import the persistent off-screen texture with its initial layout.
		rhi::RGTextureHandle offscreen = rg.ImportTexture(
			"Offscreen", offscreenColor, rhi::ImageLayout::Undefined);

		// Declare transient (graph-managed) resources.
		rhi::RGTextureHandle albedo = rg.CreateTexture({W, H, PixelFormat::RGBA8_SRGB, false, "GBufferAlbedo"});

		rhi::RGTextureHandle depth = rg.CreateTexture({W, H, PixelFormat::D32f, true, "GBufferDepth"});

		// ------------------------------------------------------------------
		// CulledPass: writes to a texture that nobody reads.
		// After Compile() it will be marked as culled.
		// ------------------------------------------------------------------
		rhi::RGTextureHandle unusedTex = rg.CreateTexture({W, H, PixelFormat::RGBA8_SRGB, false, "UnusedOutput"});

		rg.AddGraphicsPass("CulledPass", [&](rhi::RenderGraphBuilder& b)
						   { unusedTex = b.Write(unusedTex); }, [](rhi::RenderGraphContext&) {});

		// ------------------------------------------------------------------
		// GBuffer pass: fills albedo + depth.
		// ------------------------------------------------------------------
		rg.AddGraphicsPass("GBuffer", [&](rhi::RenderGraphBuilder& b)
						   {
                albedo = b.Write(albedo);
                depth  = b.WriteDepth(depth); }, [](rhi::RenderGraphContext& ctx)
						   {
							   auto& cmd = ctx.GetCommandBuffer();
							   cmd.SetViewport({0.f, 0.f,
												static_cast<float>(ctx.GetWidth()), static_cast<float>(ctx.GetHeight()),
												0.f, 1.f});
							   cmd.SetScissor({0, 0, ctx.GetWidth(), ctx.GetHeight()});
							   // In a real renderer: bind pipeline, draw scene geometry here.
						   });

		// ------------------------------------------------------------------
		// Composite pass: reads albedo, writes to the imported off-screen
		// target. Marked as a side-effect so it is never culled regardless
		// of who reads the output.
		// ------------------------------------------------------------------
		rg.AddGraphicsPass("Composite", [&](rhi::RenderGraphBuilder& b)
						   {
                albedo    = b.Read(albedo);
                offscreen = b.Write(offscreen);
                b.SetSideEffect(); }, [](rhi::RenderGraphContext& ctx)
						   {
							   auto& cmd = ctx.GetCommandBuffer();
							   cmd.SetViewport({0.f, 0.f,
												static_cast<float>(ctx.GetWidth()), static_cast<float>(ctx.GetHeight()),
												0.f, 1.f});
							   cmd.SetScissor({0, 0, ctx.GetWidth(), ctx.GetHeight()});
							   // In a real renderer: bind full-screen quad + composite shader.
						   });

		rg.SetFinalOutput(offscreen);
		rg.Compile();

		// Print the compiled graph once so we can see what was culled.
		std::cout << "\n=== RenderGraph compiled ===\n";
		rg.DumpGraph(std::cout);
		std::cout << "============================\n\n";

		// ------------------------------------------------------------------
		// Main loop
		// ------------------------------------------------------------------
		float deltaTime = 0.f;
		auto lastFrameTime = std::chrono::high_resolution_clock::now();

		while (!window->ShouldClose())
		{
			displayManager.DispatchEvents();

			const auto now = std::chrono::high_resolution_clock::now();
			deltaTime = std::chrono::duration<float>(now - lastFrameTime).count();
			lastFrameTime = now;

			rhi::Frame& frame = swapChain->AcquireFrame();

			// AcquireFrame() has waited on this slot's fence, so the GPU has
			// finished with the FrameBuffers and transient textures that were
			// recorded into the previous command buffer for this slot.
			// Releasing them here (rather than right after Present()) is
			// therefore safe.
			rg.Reset();

			rhi::CommandBuffer& cmd = frame.GetCommandBuffer();

			cmd.Reset();
			cmd.Begin();
			{
				// Execute all non-culled render graph passes.
				// Barriers are inserted automatically between passes.
				rg.Execute(cmd, W, H);

				// Present to the swapchain. In a complete renderer this would
				// blit / sample from offscreenColor; here we just clear.
				cmd.BeginRenderPass(
					*swapChain->GetRenderPass(),
					frame.GetFrameBuffer(),
					Vector3f{0.08f, 0.08f, 0.12f});
				cmd.EndRenderPass();
			}
			cmd.End();

			frame.Present();

			window->SetTitle(std::format("RenderGraph Example - {:.1f} fps", 1.f / deltaTime));
		}

		swapChain->WaitAll();
		device->WaitIdle();
	}
	catch (const std::exception& e)
	{
		Logger::Error("Unhandled exception: '{}'", e.what());
		return EXIT_FAILURE;
	}
	return 0;
}
