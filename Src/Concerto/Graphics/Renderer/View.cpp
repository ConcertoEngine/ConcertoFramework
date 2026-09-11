#include "Concerto/Graphics/Renderer/View.hpp"

namespace cct::gfx
{
	View View::FromCamera(const Camera& camera, const Vector2u& viewportSize)
	{
		View view;
		view.viewMatrix = camera.GetViewMatrix();
		view.projectionMatrix = camera.GetProjectionMatrix();
		view.viewportSize = viewportSize;
		return view;
	}
} // namespace cct::gfx
