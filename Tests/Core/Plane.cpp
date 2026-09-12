//
// Created by arthur on 13/09/2026.
//

#include "Concerto/Core/Math/Plane/Plane.hpp"

#include <catch2/catch_test_macros.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	SCENARIO("Plane - FromCoefficients normalizes the plane")
	{
		GIVEN("Coefficients describing the plane x = 2, scaled by a factor of 2")
		{
			Plane plane = Plane::FromCoefficients(2.0f, 0.0f, 0.0f, -4.0f);

			THEN("The resulting plane has a unit normal and a matching distance")
			{
				REQUIRE(plane.GetNormal() == Vector3f(1.0f, 0.0f, 0.0f));
				REQUIRE(plane.GetDistance() == -2.0f);
			}
		}
	}

	SCENARIO("Plane - SignedDistance")
	{
		GIVEN("The plane x = 2, with its normal pointing towards +X")
		{
			Plane plane = Plane::FromCoefficients(1.0f, 0.0f, 0.0f, -2.0f);

			THEN("A point on the plane has a signed distance of 0")
			{
				REQUIRE(plane.SignedDistance(Vector3f(2.0f, 5.0f, -3.0f)) == 0.0f);
			}

			THEN("A point on the normal's side has a positive signed distance")
			{
				REQUIRE(plane.SignedDistance(Vector3f(5.0f, 0.0f, 0.0f)) == 3.0f);
			}

			THEN("A point on the opposite side has a negative signed distance")
			{
				REQUIRE(plane.SignedDistance(Vector3f(0.0f, 0.0f, 0.0f)) == -2.0f);
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
