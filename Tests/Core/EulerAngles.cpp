//
// Created by arthur on 14/03/2023.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Concerto/Core/Math/EulerAngles/EulerAngles.hpp"

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;
	constexpr float near = 0.001f;

	SCENARIO("EulerAngles - construction")
	{
		GIVEN("EulerAngles(1, 2, 3)")
		{
			EulerAnglesf e(1.f, 2.f, 3.f);
			THEN("Pitch, Yaw, Roll are correct")
			{
				REQUIRE(e.Pitch() == 1.f);
				REQUIRE(e.Yaw() == 2.f);
				REQUIRE(e.Roll() == 3.f);
			}
		}
	}

	SCENARIO("EulerAngles - arithmetic operators")
	{
		GIVEN("Two EulerAngles(1, 2, 3)")
		{
			EulerAnglesf e1(1.f, 2.f, 3.f);
			EulerAnglesf e2(1.f, 2.f, 3.f);

			WHEN("Added")
			{
				EulerAnglesf e3 = e1 + e2;
				THEN("Result is (2, 4, 6)")
				{
					REQUIRE(e3.Pitch() == 2.f);
					REQUIRE(e3.Yaw() == 4.f);
					REQUIRE(e3.Roll() == 6.f);
				}
			}

			WHEN("Subtracted")
			{
				EulerAnglesf e3 = e1 - e2;
				THEN("Result is (0, 0, 0)")
				{
					REQUIRE(e3.Pitch() == 0.f);
					REQUIRE(e3.Yaw() == 0.f);
					REQUIRE(e3.Roll() == 0.f);
				}
			}

			WHEN("Multiplied")
			{
				EulerAnglesf e2r = e1 * e1;
				THEN("Result is (1, 4, 9)")
				{
					REQUIRE(e2r.Pitch() == 1.f);
					REQUIRE(e2r.Yaw() == 4.f);
					REQUIRE(e2r.Roll() == 9.f);
				}
			}

			WHEN("Divided")
			{
				EulerAnglesf e2r = e1 / e1;
				THEN("Result is (1, 1, 1)")
				{
					REQUIRE(e2r.Pitch() == 1.f);
					REQUIRE(e2r.Yaw() == 1.f);
					REQUIRE(e2r.Roll() == 1.f);
				}
			}
		}
	}

	SCENARIO("EulerAngles - compound assignment operators")
	{
		GIVEN("Two EulerAngles(1, 2, 3)")
		{
			EulerAnglesf e1(1.f, 2.f, 3.f);
			EulerAnglesf e2(1.f, 2.f, 3.f);

			WHEN("+=")
			{
				e1 += e2;
				THEN("e1 is (2, 4, 6)")
				{
					REQUIRE(e1.Pitch() == 2.f);
					REQUIRE(e1.Yaw() == 4.f);
					REQUIRE(e1.Roll() == 6.f);
				}
			}

			WHEN("-=")
			{
				e1 -= e2;
				THEN("e1 is (0, 0, 0)")
				{
					REQUIRE(e1.Pitch() == 0);
					REQUIRE(e1.Yaw() == 0);
					REQUIRE(e1.Roll() == 0);
				}
			}

			WHEN("*=")
			{
				e1 *= e1;
				THEN("e1 is (1, 4, 9)")
				{
					REQUIRE(e1.Pitch() == 1.f);
					REQUIRE(e1.Yaw() == 4.f);
					REQUIRE(e1.Roll() == 9.f);
				}
			}

			WHEN("/=")
			{
				e1 /= e1;
				THEN("e1 is (1, 1, 1)")
				{
					REQUIRE(e1.Pitch() == 1);
					REQUIRE(e1.Yaw() == 1);
					REQUIRE(e1.Roll() == 1);
				}
			}
		}
	}

	SCENARIO("EulerAngles - ToQuaternion round-trip")
	{
		GIVEN("EulerAngles(45, 25, 68)")
		{
			EulerAnglesf e1(45.f, 25.f, 68.f);
			WHEN("Converted to Quaternion and back")
			{
				Quaternionf q = e1.ToQuaternion();
				EulerAnglesf e2 = q.ToEulerAngles();
				THEN("Pitch, Yaw, Roll are within tolerance")
				{
					CHECK_THAT(e2.Pitch(), Catch::Matchers::WithinAbs(e1.Pitch(), near));
					CHECK_THAT(e2.Roll(), Catch::Matchers::WithinAbs(e1.Roll(), near));
					CHECK_THAT(e2.Yaw(), Catch::Matchers::WithinAbs(e1.Yaw(), near));
				}
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
