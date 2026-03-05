//
// Created by arthur on 06/09/2024.
//

#ifndef CONCERTO_GRAPHICS_UTILS_HPP
#define CONCERTO_GRAPHICS_UTILS_HPP

#include <Concerto/Core/Types/Types.hpp>

namespace cct::gfx
{
	struct Rect2D
	{
		Int32 x;
		Int32 y;
		UInt32 width;
		UInt32 height;
	};

	struct Viewport
	{
		float x;
		float y;
		float width;
		float height;
		float minDepth;
		float maxDepth;
	};
}

#endif //CONCERTO_GRAPHICS_UTILS_HPP
