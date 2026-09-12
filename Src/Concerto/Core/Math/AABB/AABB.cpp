//
// Created by arthur on 13/09/2026.
//

#include "Concerto/Core/Math/AABB/AABB.hpp"

#include <algorithm>
#include <array>
#include <limits>

namespace cct
{
	AABB::AABB() :
		_min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
		_max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
	{
	}

	AABB::AABB(const Vector3f& min, const Vector3f& max) :
		_min(min),
		_max(max)
	{
	}

	void AABB::Extend(const Vector3f& point)
	{
		_min.X() = std::min(_min.X(), point.X());
		_min.Y() = std::min(_min.Y(), point.Y());
		_min.Z() = std::min(_min.Z(), point.Z());
		_max.X() = std::max(_max.X(), point.X());
		_max.Y() = std::max(_max.Y(), point.Y());
		_max.Z() = std::max(_max.Z(), point.Z());
	}

	void AABB::Extend(const AABB& other)
	{
		Extend(other._min);
		Extend(other._max);
	}

	const Vector3f& AABB::GetMin() const
	{
		return _min;
	}

	const Vector3f& AABB::GetMax() const
	{
		return _max;
	}

	Vector3f AABB::GetCenter() const
	{
		return (_min + _max) * 0.5f;
	}

	Vector3f AABB::GetExtents() const
	{
		return (_max - _min) * 0.5f;
	}

	AABB AABB::Transformed(const Matrix4f& transform) const
	{
		const std::array<Vector3f, 8> corners = {
			Vector3f(_min.X(), _min.Y(), _min.Z()),
			Vector3f(_max.X(), _min.Y(), _min.Z()),
			Vector3f(_min.X(), _max.Y(), _min.Z()),
			Vector3f(_max.X(), _max.Y(), _min.Z()),
			Vector3f(_min.X(), _min.Y(), _max.Z()),
			Vector3f(_max.X(), _min.Y(), _max.Z()),
			Vector3f(_min.X(), _max.Y(), _max.Z()),
			Vector3f(_max.X(), _max.Y(), _max.Z()),
		};

		AABB result;
		for (const Vector3f& corner : corners)
		{
			Vector3f transformed(
				transform(0, 0) * corner.X() + transform(0, 1) * corner.Y() + transform(0, 2) * corner.Z() + transform(0, 3),
				transform(1, 0) * corner.X() + transform(1, 1) * corner.Y() + transform(1, 2) * corner.Z() + transform(1, 3),
				transform(2, 0) * corner.X() + transform(2, 1) * corner.Y() + transform(2, 2) * corner.Z() + transform(2, 3));
			result.Extend(transformed);
		}
		return result;
	}
} // namespace cct
