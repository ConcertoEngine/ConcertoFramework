//
// Created by arthur on 23/02/2023.
//

#include <catch2/catch_test_macros.hpp>
#include "Concerto/Core/Math/Transform/Transform.hpp"

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	SCENARIO("Transform - construction")
	{
		GIVEN("A Transform constructed with zero location, rotation, and scale")
		{
			Transform transform(Vector3f(0.f, 0.f, 0.f), Quaternionf(0.f, 0.f, 0.f, 0.f), Vector3f(0.f, 0.f, 0.f));

			THEN("GetLocation, GetRotation, and GetScale return the expected values")
			{
				REQUIRE(transform.GetLocation() == Vector3f(0.f, 0.f, 0.f));
				REQUIRE(transform.GetRotation() == Quaternionf(0.f, 0.f, 0.f, 0.f));
				REQUIRE(transform.GetScale() == Vector3f(0.f, 0.f, 0.f));
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
