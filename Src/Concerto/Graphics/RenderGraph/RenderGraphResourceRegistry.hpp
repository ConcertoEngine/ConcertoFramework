//
// Created by arthur on 11/05/2026.
//

#ifndef CONCERTO_GRAPHICS_RHI_RENDERGRAPH_RESOURCE_REGISTRY_HPP
#define CONCERTO_GRAPHICS_RHI_RENDERGRAPH_RESOURCE_REGISTRY_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include "Concerto/Graphics/RenderGraph/RenderGraphResource.hpp"
#include "Concerto/Graphics/RHI/Buffer.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Texture.hpp"

namespace cct::gfx::rhi
{
	class Device;

	class CONCERTO_GRAPHICS_RHI_BASE_API RenderGraphResourceRegistry
	{
	public:
		struct TextureTransientSnapshot
		{
			RGTextureDesc desc;
			std::shared_ptr<Texture> physical;
		};

		struct BufferTransientSnapshot
		{
			RGBufferDesc desc;
			std::shared_ptr<Buffer> physical;
		};

		RGTextureHandle RegisterTexture(const RGTextureDesc& desc);
		RGBufferHandle RegisterBuffer(const RGBufferDesc& desc);

		RGTextureHandle ImportTexture(const char* name,
									  std::shared_ptr<Texture> texture,
									  ImageLayout currentLayout,
									  PixelFormat format = PixelFormat::RGBA8_SRGB,
									  UInt32 width = 0,
									  UInt32 height = 0);
		RGBufferHandle ImportBuffer(const char* name,
									std::shared_ptr<Buffer> buffer);

		void Allocate(Device& device);

		// Extract physical resources from transient entries into caller-owned snapshots
		// and clear the entries so the next Allocate() can re-populate them.
		// Called by RenderGraph::Reset() to hand ownership to the pending-frame deque.
		void ExtractTransients(std::vector<TextureTransientSnapshot>& outTextures,
							   std::vector<BufferTransientSnapshot>& outBuffers);

		// Return previously-extracted snapshots to the pool.
		// Safe to call once the GPU fence for those resources has been waited on.
		void ReturnTransientsToPool(std::vector<TextureTransientSnapshot>& textures,
									std::vector<BufferTransientSnapshot>& buffers);

		void Reset();

		// Clear all registered texture and buffer entries (imported and transient
		// descriptors) while keeping pool buckets for reuse on the next Allocate().
		void ClearEntries();

		[[nodiscard]] ImageLayout GetCurrentLayout(RGTextureHandle handle) const;
		void SetCurrentLayout(RGTextureHandle handle, ImageLayout layout);

		// Declare the layout a texture must be in after the last render graph pass that writes it.
		// RenderGraph::Execute() emits the required barrier at the end of the frame.
		void SetExportLayout(RGTextureHandle handle, ImageLayout layout);
		[[nodiscard]] const std::unordered_map<UInt16, std::pair<RGTextureHandle, ImageLayout>>& ExportLayouts() const
		{
			return m_exportLayouts;
		}

		Texture& GetTexture(RGTextureHandle handle);
		Buffer& GetBuffer(RGBufferHandle handle);
		const RGTextureDesc& GetDesc(RGTextureHandle handle) const;
		bool IsImported(RGTextureHandle handle) const;
		bool IsImported(RGBufferHandle handle) const;

		[[nodiscard]] UInt32 TextureCount() const
		{
			return static_cast<UInt32>(m_textures.size());
		}
		[[nodiscard]] UInt32 BufferCount() const
		{
			return static_cast<UInt32>(m_buffers.size());
		}

	private:
		struct TextureEntry
		{
			RGTextureDesc desc;
			std::shared_ptr<Texture> physical;
			ImageLayout currentLayout = ImageLayout::Undefined;
			ImageLayout initialLayout = ImageLayout::Undefined;
			bool imported = false;
			UInt16 version = 0;
		};

		struct BufferEntry
		{
			RGBufferDesc desc;
			std::shared_ptr<Buffer> physical;
			bool imported = false;
			UInt16 version = 0;
		};

		static size_t HashDesc(const RGTextureDesc& desc);
		static size_t HashDesc(const RGBufferDesc& desc);

		std::vector<TextureEntry> m_textures;
		std::vector<BufferEntry> m_buffers;
		std::unordered_map<size_t, std::vector<std::shared_ptr<Texture>>> m_transientTexturePool;
		std::unordered_map<size_t, std::vector<std::shared_ptr<Buffer>>> m_transientBufferPool;
		std::unordered_map<UInt16, std::pair<RGTextureHandle, ImageLayout>> m_exportLayouts;
	};
} // namespace cct::gfx::rhi

#endif // CONCERTO_GRAPHICS_RHI_RENDERGRAPH_RESOURCE_REGISTRY_HPP
