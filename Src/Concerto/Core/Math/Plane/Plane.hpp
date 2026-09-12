//
// Created by arthur on 13/09/2026.
//

#ifndef CONCERTO_CORE_MATH_PLANE_HPP
#define CONCERTO_CORE_MATH_PLANE_HPP

#include "Concerto/Core/Defines.hpp"
#include "Concerto/Core/Math/Vector/Vector.hpp"

namespace cct
{
	class CCT_CORE_PUBLIC_API Plane
	{
	public:
		Plane() = default;
		Plane(const Vector3f& normal, float distance);
		Plane(Plane&&) = default;
		Plane(const Plane&) = default;
		Plane& operator=(Plane&&) = default;
		Plane& operator=(const Plane&) = default;
		~Plane() = default;

		[[nodiscard]] static Plane FromCoefficients(float a, float b, float c, float d);

		[[nodiscard]] const Vector3f& GetNormal() const;
		[[nodiscard]] float GetDistance() const;
		[[nodiscard]] float SignedDistance(const Vector3f& point) const;

	private:
		Vector3f _normal;
		float _distance = 0.0f;
	};
} // namespace cct

#endif //CONCERTO_CORE_MATH_PLANE_HPP
