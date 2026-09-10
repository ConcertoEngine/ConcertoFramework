#include "Concerto/Graphics/Renderer/View.hpp"

namespace cct::gfx
{
	View View::FromCamera(const Camera& camera, const Vector2u& viewportSize)
	{
		View view;
		view.viewMatrix = camera.viewMatrix;
		view.projectionMatrix = camera.projectionMatrix;
		view.viewportSize = viewportSize;
		return view;
	}
} // namespace cct::gfx
