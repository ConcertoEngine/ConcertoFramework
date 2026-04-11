//
// Created by arthur on 10/06/22.
//

#ifndef CONCERTO_GRAPHICS_VIEWPORT_HPP
#define CONCERTO_GRAPHICS_VIEWPORT_HPP

namespace cct::gfx::vk
{
	struct Viewport
	{
		float x;
		float y;
		float width;
		float height;
		float minDepth;
		float maxDepth;
	};

} // cct::gfx::vk

#endif //CONCERTO_GRAPHICS_VIEWPORT_HPP