//
// Created by arthur on 13/09/2026.
//

#include "Concerto/Core/Math/Frustum/Frustum.hpp"

namespace cct
{
	Frustum Frustum::FromViewProjection(const Matrix4f& viewProjection)
	{
		const Matrix4f& m = viewProjection;
		Frustum frustum;

		frustum._planes[static_cast<std::size_t>(Side::Left)] = Plane::FromCoefficients(
			m(3, 0) + m(0, 0), m(3, 1) + m(0, 1), m(3, 2) + m(0, 2), m(3, 3) + m(0, 3));

		frustum._planes[static_cast<std::size_t>(Side::Right)] = Plane::FromCoefficients(
			m(3, 0) - m(0, 0), m(3, 1) - m(0, 1), m(3, 2) - m(0, 2), m(3, 3) - m(0, 3));

		frustum._planes[static_cast<std::size_t>(Side::Bottom)] = Plane::FromCoefficients(
			m(3, 0) + m(1, 0), m(3, 1) + m(1, 1), m(3, 2) + m(1, 2), m(3, 3) + m(1, 3));

		frustum._planes[static_cast<std::size_t>(Side::Top)] = Plane::FromCoefficients(
			m(3, 0) - m(1, 0), m(3, 1) - m(1, 1), m(3, 2) - m(1, 2), m(3, 3) - m(1, 3));

		frustum._planes[static_cast<std::size_t>(Side::Near)] = Plane::FromCoefficients(
			m(2, 0), m(2, 1), m(2, 2), m(2, 3));

		frustum._planes[static_cast<std::size_t>(Side::Far)] = Plane::FromCoefficients(
			m(3, 0) - m(2, 0), m(3, 1) - m(2, 1), m(3, 2) - m(2, 2), m(3, 3) - m(2, 3));

		return frustum;
	}

	const Plane& Frustum::GetPlane(Side side) const
	{
		return _planes[static_cast<std::size_t>(side)];
	}

	bool Frustum::ContainsPoint(const Vector3f& point) const
	{
		for (const Plane& plane : _planes)
		{
			if (plane.SignedDistance(point) < 0.0f)
				return false;
		}
		return true;
	}

	bool Frustum::ContainsSphere(const Vector3f& center, float radius) const
	{
		for (const Plane& plane : _planes)
		{
			if (plane.SignedDistance(center) < -radius)
				return false;
		}
		return true;
	}
} // namespace cct
