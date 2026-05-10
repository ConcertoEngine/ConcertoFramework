//
// Created by arthur on 07/08/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_TEXTUREBUILDER_HPP
#define CONCERTO_GRAPHICS_RHI_TEXTUREBUILDER_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"

namespace cct::gfx::rhi
{
	class Buffer;
	class Device;
	class CommandPool;

	class CONCERTO_GRAPHICS_RHI_BASE_API TextureBuilder
	{
	public:
		TextureBuilder(Device& device);
		~TextureBuilder();

		TextureBuilder(TextureBuilder&&) = default;
		TextureBuilder(const TextureBuilder&) = delete;

		TextureBuilder& operator=(TextureBuilder&&) = default;
		TextureBuilder& operator=(const TextureBuilder&) = delete;

		static TextureBuilder& Instance();

		std::shared_ptr<Texture> BuildTexture(const std::string& path);
		std::shared_ptr<Texture> BuildTextureFromMemory(const std::byte* pixels, UInt32 width, UInt32 height, PixelFormat format);
		void Commit();
	private:
		Device& m_device;
		std::unique_ptr<CommandPool> m_commandPool;
		std::unique_ptr<CommandPool> m_secondaryCommandPool;
		static TextureBuilder* s_instance;
		ThreadSafeHashMap<std::string, std::shared_ptr<Texture>> m_texturesCache;
		using PendingUpload = std::pair<std::shared_ptr<Texture>, std::unique_ptr<Buffer>>;
		ThreadSafeHashMap<size_t, std::unique_ptr<Buffer>> m_pendingUploads;
	};
}

#endif //CONCERTO_GRAPHICS_RHI_TEXTUREBUILDER_HPP