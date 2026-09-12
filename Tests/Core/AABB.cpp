//
// Created by arthur on 13/09/2026.
//

#include "Concerto/Core/Math/AABB/AABB.hpp"

#include <catch2/catch_test_macros.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	SCENARIO("AABB - Extend grows the box to contain a point")
	{
		GIVEN("An empty AABB")
		{
			AABB aabb;

			WHEN("Extended with a few points")
			{
				aabb.Extend(Vector3f(-1.0f, 2.0f, 0.0f));
				aabb.Extend(Vector3f(3.0f, -2.0f, 1.0f));

				THEN("Its min and max match the extremes of those points")
				{
					REQUIRE(aabb.GetMin() == Vector3f(-1.0f, -2.0f, 0.0f));
					REQUIRE(aabb.GetMax() == Vector3f(3.0f, 2.0f, 1.0f));
				}
			}
		}
	}

	SCENARIO("AABB - GetCenter and GetExtents")
	{
		GIVEN("A box from (0,0,0) to (2,4,6)")
		{
			AABB aabb(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(2.0f, 4.0f, 6.0f));

			THEN("Its center and half-extents match")
			{
				REQUIRE(aabb.GetCenter() == Vector3f(1.0f, 2.0f, 3.0f));
				REQUIRE(aabb.GetExtents() == Vector3f(1.0f, 2.0f, 3.0f));
			}
		}
	}

	SCENARIO("AABB - Transformed applies a translation")
	{
		GIVEN("A unit box centered on the origin, translated by (10, 0, 0)")
		{
			AABB aabb(Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(1.0f, 1.0f, 1.0f));
			Matrix4f translation = Vector3f(10.0f, 0.0f, 0.0f).ToTranslationMatrix();

			THEN("The transformed box is shifted by the same translation")
			{
				AABB transformed = aabb.Transformed(translation);
				REQUIRE(transformed.GetMin() == Vector3f(9.0f, -1.0f, -1.0f));
				REQUIRE(transformed.GetMax() == Vector3f(11.0f, 1.0f, 1.0f));
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
