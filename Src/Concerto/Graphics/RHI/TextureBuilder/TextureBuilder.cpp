//
// Created by arthur on 17/02/2023.
//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Concerto/Core/DeferredExit/DeferredExit.hpp>

#include "Concerto/Graphics/RHI/TextureBuilder/TextureBuilder.hpp"
#include "Concerto/Graphics/RHI/Device.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"

namespace cct::gfx::rhi
{
	TextureBuilder::TextureBuilder(Device& device) :
		m_device(device)
	{
	}

	std::shared_ptr<Texture> TextureBuilder::BuildTexture(const std::string& path)
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();
		static int i = 0;
		Int32 width, height, channels;
		stbi_uc* pixels;
		{
			CCT_GFX_PROFILER_SCOPE("stbi_load");
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

		PixelFormat format = PixelFormat::RGBA8uNorm;
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
			CCT_GFX_PROFILER_SCOPE("Map & memcpy to Buffer");
			Byte* data = nullptr;
			if (buffer->Map(&data) == false)
			{
				CCT_ASSERT_FALSE("Cannot map buffer");
				return nullptr;
			}
			
			std::memcpy(data, pixels, allocationSize);
			buffer->UnMap();
		}

		auto texture = BuildApiTexture(format, width, height);
		if (texture == nullptr)
			return nullptr;

		m_buffersToUpload.emplace(texture, std::move(buffer));
		m_texturesCache.emplace(path, texture);
		++i;
		return texture;
	}

	void TextureBuilder::Commit()
	{
		CCT_GFX_AUTO_PROFILER_SCOPE();

		for (auto& [texture, buffer] : m_buffersToUpload)
			buffer->CopyTo(*texture);

		InternalCommit();

		m_buffersToUpload.clear();
	}
}
