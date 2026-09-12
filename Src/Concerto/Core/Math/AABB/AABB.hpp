//
// Created by arthur on 13/09/2026.
//

#ifndef CONCERTO_CORE_MATH_AABB_HPP
#define CONCERTO_CORE_MATH_AABB_HPP

#include "Concerto/Core/Defines.hpp"
#include "Concerto/Core/Math/Matrix/Matrix.hpp"
#include "Concerto/Core/Math/Vector/Vector.hpp"

namespace cct
{
	class CCT_CORE_PUBLIC_API AABB
	{
	public:
		AABB();
		AABB(const Vector3f& min, const Vector3f& max);
		AABB(AABB&&) = default;
		AABB(const AABB&) = default;
		AABB& operator=(AABB&&) = default;
		AABB& operator=(const AABB&) = default;
		~AABB() = default;

		void Extend(const Vector3f& point);
		void Extend(const AABB& other);

		[[nodiscard]] const Vector3f& GetMin() const;
		[[nodiscard]] const Vector3f& GetMax() const;
		[[nodiscard]] Vector3f GetCenter() const;
		[[nodiscard]] Vector3f GetExtents() const;

		[[nodiscard]] AABB Transformed(const Matrix4f& transform) const;

	private:
		Vector3f _min;
		Vector3f _max;
	};
} // namespace cct

#endif //CONCERTO_CORE_MATH_AABB_HPP
