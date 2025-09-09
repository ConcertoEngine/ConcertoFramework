//
// Created by arthur on 12/05/2024.
//

#ifndef CONCERTO_GRAPHICS_RHI_TEXTURE_HPP
#define CONCERTO_GRAPHICS_RHI_TEXTURE_HPP

#include <memory>
#include "Concerto/Graphics/RHI/Defines.hpp"
#include "Concerto/Graphics/RHI/Enums.hpp"

namespace cct::gfx::rhi
{
	class TextureView;

	class CONCERTO_GRAPHICS_RHI_BASE_API Texture
	{
	public:
		virtual ~Texture() = default;
		virtual std::unique_ptr<TextureView> CreateView() const { return nullptr; }
	};

	class CONCERTO_GRAPHICS_RHI_BASE_API TextureView
	{
	public:
		virtual ~TextureView() = default;
	};
}


#endif //CONCERTO_GRAPHICS_RHI_TEXTURE_HPP