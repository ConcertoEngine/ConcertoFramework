//
// Created by arthur on 15/04/2023.
//

#ifndef CONCERTO_GRAPHICS_PRIMITIVES_HPP
#define CONCERTO_GRAPHICS_PRIMITIVES_HPP

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/Core/Vertex.hpp"

namespace cct::gfx
{
	struct CONCERTO_GRAPHICS_CORE_API Primitive
	{
		static Vertices MakeCone(float radius, float height, UInt32 slices);
		static Vertices MakeCube(float size);
		static Vertices MakeSphere(float radius, UInt32 slices, UInt32 stacks);
		static Vertices MakeCylinder(float radius, float height, UInt32 slices);
		static Vertices MakePlane(float size);
	};
}
#endif //CONCERTO_GRAPHICS_PRIMITIVES_HPP
