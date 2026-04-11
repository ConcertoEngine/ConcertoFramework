//
// Created by arthur on 08/08/2024.
//

#ifndef CONCERTO_GRAPHICS_VERTEX_HPP
#define CONCERTO_GRAPHICS_VERTEX_HPP

#include <vector>
#include <Concerto/Core/Math/Vector/Vector.hpp>

namespace cct::gfx
{
	struct Vertex
	{
		Vector3f position;
		Vector3f normal;
		Vector3f color;
		Vector2f uv;
	};
	using Vertices = std::vector<Vertex>;
}

#endif //CONCERTO_GRAPHICS_VERTEX_HPP