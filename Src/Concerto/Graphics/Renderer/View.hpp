#ifndef CONCERTO_GRAPHICS_RENDERER_VIEW_HPP
#define CONCERTO_GRAPHICS_RENDERER_VIEW_HPP

#include <Concerto/Core/Math/Matrix/Matrix.hpp>
#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/Core/Camera/Camera.hpp"
#include "Concerto/Graphics/RHI/Defines.hpp"

namespace cct::gfx
{
	struct CONCERTO_GRAPHICS_RHI_BASE_API View
	{
		Matrix4f viewMatrix;
		Matrix4f projectionMatrix;
		Vector2u viewportSize;

		static View FromCamera(const Camera& camera, const Vector2u& viewportSize);
	};
} // namespace cct::gfx

#endif // CONCERTO_GRAPHICS_RENDERER_VIEW_HPP
