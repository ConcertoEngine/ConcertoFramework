//
// Created by arthur on 13/09/2026.
//

#include "Concerto/Core/Math/Frustum/Frustum.hpp"

#include <catch2/catch_test_macros.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	SCENARIO("Frustum - FromViewProjection with the identity matrix matches the canonical clip volume")
	{
		GIVEN("A frustum built from the identity view-projection matrix")
		{
			Frustum frustum = Frustum::FromViewProjection(Matrix4f::Identity());

			THEN("A point at the center of the clip volume is contained")
			{
				REQUIRE(frustum.ContainsPoint(Vector3f(0.0f, 0.0f, 0.5f)));
			}

			THEN("A point outside the left/right clip planes is not contained")
			{
				REQUIRE_FALSE(frustum.ContainsPoint(Vector3f(-1.5f, 0.0f, 0.5f)));
				REQUIRE_FALSE(frustum.ContainsPoint(Vector3f(1.5f, 0.0f, 0.5f)));
			}

			THEN("A point outside the bottom/top clip planes is not contained")
			{
				REQUIRE_FALSE(frustum.ContainsPoint(Vector3f(0.0f, -1.5f, 0.5f)));
				REQUIRE_FALSE(frustum.ContainsPoint(Vector3f(0.0f, 1.5f, 0.5f)));
			}

			THEN("A point outside the near/far clip planes is not contained")
			{
				REQUIRE_FALSE(frustum.ContainsPoint(Vector3f(0.0f, 0.0f, -0.5f)));
				REQUIRE_FALSE(frustum.ContainsPoint(Vector3f(0.0f, 0.0f, 1.5f)));
			}
		}
	}

	SCENARIO("Frustum - ContainsSphere")
	{
		GIVEN("A frustum built from the identity view-projection matrix")
		{
			Frustum frustum = Frustum::FromViewProjection(Matrix4f::Identity());

			THEN("A sphere fully inside the clip volume is contained")
			{
				REQUIRE(frustum.ContainsSphere(Vector3f(0.0f, 0.0f, 0.5f), 0.25f));
			}

			THEN("A sphere overlapping a clip plane is still contained")
			{
				REQUIRE(frustum.ContainsSphere(Vector3f(1.2f, 0.0f, 0.5f), 0.5f));
			}

			THEN("A sphere fully outside a clip plane is not contained")
			{
				REQUIRE_FALSE(frustum.ContainsSphere(Vector3f(2.0f, 0.0f, 0.5f), 0.5f));
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
