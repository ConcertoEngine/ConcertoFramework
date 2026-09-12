//
// Created by arthur on 13/09/2026.
//

#include "Concerto/Core/Math/Plane/Plane.hpp"

namespace cct
{
	Plane::Plane(const Vector3f& normal, float distance) :
		_normal(normal),
		_distance(distance)
	{
	}

	Plane Plane::FromCoefficients(float a, float b, float c, float d)
	{
		Vector3f normal(a, b, c);
		const float length = normal.Length();
		return Plane(normal / length, d / length);
	}

	const Vector3f& Plane::GetNormal() const
	{
		return _normal;
	}

	float Plane::GetDistance() const
	{
		return _distance;
	}

	float Plane::SignedDistance(const Vector3f& point) const
	{
		return _normal.Dot(point) + _distance;
	}
} // namespace cct
