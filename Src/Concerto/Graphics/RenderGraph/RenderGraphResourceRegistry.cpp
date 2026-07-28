//
// Created by arthur on 11/05/2026.
//

#include "Concerto/Graphics/RenderGraph/RenderGraphResourceRegistry.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Graphics/RHI/Device.hpp"

namespace cct::gfx::rhi
{
	RGTextureHandle RenderGraphResourceRegistry::RegisterTexture(const RGTextureDesc& desc)
	{
		const UInt16 idx = static_cast<UInt16>(m_textures.size());
		m_textures.push_back({desc, nullptr, ImageLayout::Undefined, ImageLayout::Undefined, false, 0});
		return {idx, 0};
	}

	RGBufferHandle RenderGraphResourceRegistry::RegisterBuffer(const RGBufferDesc& desc)
	{
		const UInt16 idx = static_cast<UInt16>(m_buffers.size());
		m_buffers.push_back({desc, nullptr, false, 0});
		return {idx, 0};
	}

	RGTextureHandle RenderGraphResourceRegistry::ImportTexture(const char* name,
															   std::shared_ptr<Texture> texture,
															   ImageLayout currentLayout,
															   PixelFormat format,
															   UInt32 width,
															   UInt32 height)
	{
		const UInt16 idx = static_cast<UInt16>(m_textures.size());
		RGTextureDesc desc;
		desc.name = name;
		desc.format = format;
		desc.width = width;
		desc.height = height;
		m_textures.push_back({desc, std::move(texture), currentLayout, currentLayout, true, 0});
		return {idx, 0};
	}

	RGBufferHandle RenderGraphResourceRegistry::ImportBuffer(const char* name,
															 std::shared_ptr<Buffer> buffer)
	{
		const UInt16 idx = static_cast<UInt16>(m_buffers.size());
		RGBufferDesc desc;
		desc.name = name;
		m_buffers.push_back({desc, std::move(buffer), true, 0});
		return {idx, 0};
	}

	void RenderGraphResourceRegistry::Allocate(Device& device)
	{
		for (auto& entry : m_textures)
		{
			if (entry.imported || entry.physical)
				continue;

			const size_t hash = HashDesc(entry.desc);
			auto& pool = m_transientTexturePool[hash];
			if (!pool.empty())
			{
				entry.physical = pool.back();
				pool.pop_back();
			}
			else
			{
				entry.physical = device.CreateTexture(
					entry.desc.format,
					static_cast<Int32>(entry.desc.width),
					static_cast<Int32>(entry.desc.height));
				CCT_ASSERT(entry.physical != nullptr,
						   "RenderGraphResourceRegistry::Allocate: CreateTexture failed for '{}'", entry.desc.name);
			}
		}

		for (auto& entry : m_buffers)
		{
			if (entry.imported || entry.physical)
				continue;

			const size_t hash = HashDesc(entry.desc);
			auto& pool = m_transientBufferPool[hash];
			if (!pool.empty())
			{
				entry.physical = pool.back();
				pool.pop_back();
			}
			else
			{
				auto uniqueBuf = device.CreateBuffer(entry.desc.usage, entry.desc.size, false);
				CCT_ASSERT(uniqueBuf != nullptr,
						   "RenderGraphResourceRegistry::Allocate: CreateBuffer failed for '{}'", entry.desc.name);
				entry.physical = std::shared_ptr<Buffer>(std::move(uniqueBuf));
			}
		}
	}

	void RenderGraphResourceRegistry::ExtractTransients(
		std::vector<RenderGraphResourceRegistry::TextureTransientSnapshot>& outTextures,
		std::vector<RenderGraphResourceRegistry::BufferTransientSnapshot>& outBuffers)
	{
		for (auto& entry : m_textures)
		{
			if (!entry.imported && entry.physical)
			{
				outTextures.push_back({entry.desc, std::move(entry.physical)});
				entry.currentLayout = ImageLayout::Undefined;
			}
		}

		for (auto& entry : m_buffers)
		{
			if (!entry.imported && entry.physical)
			{
				outBuffers.push_back({entry.desc, std::move(entry.physical)});
			}
		}
	}

	void RenderGraphResourceRegistry::ReturnTransientsToPool(
		std::vector<RenderGraphResourceRegistry::TextureTransientSnapshot>& textures,
		std::vector<RenderGraphResourceRegistry::BufferTransientSnapshot>& buffers)
	{
		// Bounded pool: without a cap, repeated topology rebuilds would accumulate
		// retired resources forever (each rebuild allocates fresh transients while the
		// previous generation is still fence-protected).
		constexpr std::size_t kMaxPooledPerDesc = 8;

		for (auto& snap : textures)
		{
			auto& bucket = m_transientTexturePool[HashDesc(snap.desc)];
			if (bucket.size() < kMaxPooledPerDesc)
				bucket.push_back(std::move(snap.physical));
		}
		for (auto& snap : buffers)
		{
			auto& bucket = m_transientBufferPool[HashDesc(snap.desc)];
			if (bucket.size() < kMaxPooledPerDesc)
				bucket.push_back(std::move(snap.physical));
		}
		textures.clear();
		buffers.clear();
	}

	void RenderGraphResourceRegistry::ClearEntries()
	{
		m_textures.clear();
		m_buffers.clear();
		m_exportLayouts.clear();
	}

	void RenderGraphResourceRegistry::SetExportLayout(RGTextureHandle handle, ImageLayout layout)
	{
		m_exportLayouts[handle.index] = {handle, layout};
	}

	void RenderGraphResourceRegistry::Reset()
	{
		// Transient entries keep their physical resource and tracked layout across frames:
		// the handle->physical assignment is stable until ClearEntries(), so descriptor sets
		// bound to a transient texture stay valid between rebuilds.
		for (auto& entry : m_textures)
		{
			if (entry.imported)
				entry.currentLayout = entry.initialLayout;
		}
	}

	ImageLayout RenderGraphResourceRegistry::GetCurrentLayout(RGTextureHandle handle) const
	{
		CCT_ASSERT(handle.index < m_textures.size(),
				   "RenderGraphResourceRegistry::GetCurrentLayout: invalid handle index {}", handle.index);
		return m_textures[handle.index].currentLayout;
	}

	void RenderGraphResourceRegistry::SetCurrentLayout(RGTextureHandle handle, ImageLayout layout)
	{
		CCT_ASSERT(handle.index < m_textures.size(),
				   "RenderGraphResourceRegistry::SetCurrentLayout: invalid handle index {}", handle.index);
		m_textures[handle.index].currentLayout = layout;
	}

	Texture& RenderGraphResourceRegistry::GetTexture(RGTextureHandle handle)
	{
		CCT_ASSERT(handle.index < m_textures.size() && m_textures[handle.index].physical != nullptr,
				   "RenderGraphResourceRegistry::GetTexture: invalid or unallocated handle index {}", handle.index);
		return *m_textures[handle.index].physical;
	}

	Buffer& RenderGraphResourceRegistry::GetBuffer(RGBufferHandle handle)
	{
		CCT_ASSERT(handle.index < m_buffers.size() && m_buffers[handle.index].physical != nullptr,
				   "RenderGraphResourceRegistry::GetBuffer: invalid or unallocated handle index {}", handle.index);
		return *m_buffers[handle.index].physical;
	}

	const RGTextureDesc& RenderGraphResourceRegistry::GetDesc(RGTextureHandle handle) const
	{
		CCT_ASSERT(handle.index < m_textures.size(),
				   "RenderGraphResourceRegistry::GetDesc: invalid handle index {}", handle.index);
		return m_textures[handle.index].desc;
	}

	bool RenderGraphResourceRegistry::IsImported(RGTextureHandle handle) const
	{
		CCT_ASSERT(handle.index < m_textures.size(),
				   "RenderGraphResourceRegistry::IsImported: invalid handle index {}", handle.index);
		return m_textures[handle.index].imported;
	}

	bool RenderGraphResourceRegistry::IsImported(RGBufferHandle handle) const
	{
		CCT_ASSERT(handle.index < m_buffers.size(),
				   "RenderGraphResourceRegistry::IsImported: invalid handle index {}", handle.index);
		return m_buffers[handle.index].imported;
	}

	size_t RenderGraphResourceRegistry::HashDesc(const RGTextureDesc& desc)
	{
		size_t h = std::hash<UInt32>{}(desc.width);
		h ^= std::hash<UInt32>{}(desc.height) + 0x9e3779b9u + (h << 6) + (h >> 2);
		h ^= std::hash<UInt8>{}(static_cast<UInt8>(desc.format)) + 0x9e3779b9u + (h << 6) + (h >> 2);
		h ^= std::hash<bool>{}(desc.isDepth) + 0x9e3779b9u + (h << 6) + (h >> 2);
		return h;
	}

	size_t RenderGraphResourceRegistry::HashDesc(const RGBufferDesc& desc)
	{
		size_t h = std::hash<UInt32>{}(desc.size);
		h ^= std::hash<UInt32>{}(desc.usage.Value()) + 0x9e3779b9u + (h << 6) + (h >> 2);
		return h;
	}
} // namespace cct::gfx::rhi
