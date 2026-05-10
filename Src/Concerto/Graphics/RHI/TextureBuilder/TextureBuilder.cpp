//
// Created by arthur on 17/02/2023.
//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Concerto/Core/DeferredExit/DeferredExit.hpp>

#include "Concerto/Graphics/RHI/TextureBuilder/TextureBuilder.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/CommandPool.hpp"
#include "Concerto/Graphics/RHI/CommandBuffer.hpp"
#include "Concerto/Graphics/RHI/Queue.hpp"
#include "Concerto/Graphics/RHI/Fence.hpp"

namespace cct::gfx::rhi
{
	TextureBuilder*	TextureBuilder::s_instance = nullptr;

	TextureBuilder::TextureBuilder(Device& device) :
		m_device(device),
		m_commandPool(device.CreateCommandPool(QueueFamily::Graphics, CommandBufferUsage::Primary)),
		m_secondaryCommandPool(device.CreateCommandPool(QueueFamily::Graphics, CommandBufferUsage::Secondary))
	{
		s_instance = this;
	}

	TextureBuilder::~TextureBuilder()
	{
		s_instance = nullptr;
	}

	TextureBuilder& TextureBuilder::Instance()
	{
		return *s_instance;
	}

	std::shared_ptr<Texture> TextureBuilder::BuildTexture(const std::string& path)
	{
		CCT_PROFILER_SCOPE();
		Int32 width, height, channels;
		stbi_uc* pixels;
		{
			CCT_PROFILER_SCOPE("stbi_load");
			pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
			if (!pixels)
			{
				CCT_ASSERT_FALSE("ConcertoGraphics: Failed to load texture '{}', error: '{}'", path, stbi_failure_reason());
				return nullptr;
			}
		}

		DeferredExit m_([&]()
		{
			stbi_image_free(pixels);
		});

		PixelFormat format = PixelFormat::RGBA8_SRGB;
		if (channels == 3)
		{
			channels = 4; //tmp fix
			//format = PixelFormat::RGB8uNorm;
		}

		UInt32 allocationSize = width * height * channels;
		auto buffer = m_device.CreateBuffer(static_cast<rhi::BufferUsageFlags>(BufferUsage::TransferSrc), allocationSize, true);
		if (buffer == nullptr)
			return nullptr;

		{
			CCT_PROFILER_SCOPE("Map & memcpy to Buffer");
			Byte* data = nullptr;
			if (buffer->Map(&data) == false)
			{
				CCT_ASSERT_FALSE("Cannot map buffer");
				return nullptr;
			}

			std::memcpy(data, pixels, allocationSize);
			buffer->UnMap();
		}

		auto texture = m_device.CreateTexture(format, width, height);
		if (texture == nullptr)
			return nullptr;

		// Store pending upload with texture pointer as key
		m_pendingUploads.insert_or_assign(reinterpret_cast<size_t>(texture.get()), std::move(buffer));
		m_texturesCache.emplace(path, texture);
		return texture;
	}

	std::shared_ptr<Texture> TextureBuilder::BuildTextureFromMemory(
		const std::byte* pixels, UInt32 width, UInt32 height, PixelFormat format)
	{
		const UInt32 allocationSize = width * height * 4;
		auto buffer = m_device.CreateBuffer(
			static_cast<rhi::BufferUsageFlags>(BufferUsage::TransferSrc), allocationSize, true);
		if (!buffer)
			return nullptr;

		Byte* data = nullptr;
		if (!buffer->Map(&data))
			return nullptr;
		std::memcpy(data, pixels, allocationSize);
		buffer->UnMap();

		auto texture = m_device.CreateTexture(format, static_cast<Int32>(width), static_cast<Int32>(height));
		if (!texture)
			return nullptr;

		m_pendingUploads.insert_or_assign(reinterpret_cast<size_t>(texture.get()), std::move(buffer));
		return texture;
	}

	void TextureBuilder::Commit()
	{
		CCT_PROFILER_SCOPE();

		if (m_pendingUploads.empty())
			return;

		auto cmdBuf = m_commandPool->AllocateCommandBuffer();
		cmdBuf->Begin();

		// Iterate through all pending uploads
		// Note: This captures a snapshot; new uploads during iteration won't be processed
		std::vector<std::pair<size_t, std::unique_ptr<Buffer>>> uploads;
		for (auto& [texturePtr, buffer] : m_pendingUploads)
		{
			uploads.emplace_back(texturePtr, std::move(buffer));
		}

		for (auto& [texturePtr, buffer] : uploads)
		{
			auto texture = reinterpret_cast<Texture*>(texturePtr);
			cmdBuf->TransitionImageLayout(*texture, ImageLayout::Undefined, ImageLayout::TransferDstOptimal);
			cmdBuf->Copy(*buffer, *texture);
			cmdBuf->TransitionImageLayout(*texture, ImageLayout::TransferDstOptimal, ImageLayout::ShaderReadOnlyOptimal);
		}

		cmdBuf->End();

		auto fence = m_device.CreateFence();
		m_device.GetQueue(QueueFamily::Graphics).Submit(*cmdBuf, fence.get());
		fence->Wait();

		m_pendingUploads.clear();
	}
}
