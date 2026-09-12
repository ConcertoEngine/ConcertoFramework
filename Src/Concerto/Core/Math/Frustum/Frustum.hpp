//
// Created by arthur on 13/09/2026.
//

#ifndef CONCERTO_CORE_MATH_FRUSTUM_HPP
#define CONCERTO_CORE_MATH_FRUSTUM_HPP

#include <array>

#include "Concerto/Core/Defines.hpp"
#include "Concerto/Core/Math/Matrix/Matrix.hpp"
#include "Concerto/Core/Math/Plane/Plane.hpp"
#include "Concerto/Core/Math/Vector/Vector.hpp"

namespace cct
{
	class CCT_CORE_PUBLIC_API Frustum
	{
	public:
		enum class Side
		{
			Left,
			Right,
			Bottom,
			Top,
			Near,
			Far,
			Count
		};

		Frustum() = default;
		Frustum(Frustum&&) = default;
		Frustum(const Frustum&) = default;
		Frustum& operator=(Frustum&&) = default;
		Frustum& operator=(const Frustum&) = default;
		~Frustum() = default;

		[[nodiscard]] static Frustum FromViewProjection(const Matrix4f& viewProjection);

		[[nodiscard]] const Plane& GetPlane(Side side) const;
		[[nodiscard]] bool ContainsPoint(const Vector3f& point) const;
		[[nodiscard]] bool ContainsSphere(const Vector3f& center, float radius) const;

	private:
		std::array<Plane, static_cast<std::size_t>(Side::Count)> _planes;
	};
} // namespace cct

#endif //CONCERTO_CORE_MATH_FRUSTUM_HPP
