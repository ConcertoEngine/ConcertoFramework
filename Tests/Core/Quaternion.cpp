//
// Created by arthur on 04/09/2022.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Concerto/Core/Math/Quaternion/Quaternion.hpp"

namespace CCT_ANONYMOUS_NAMESPACE
{
	// results are from https://www.redcrab-software.com/en/Calculator/Quaternion-Calculator
	constexpr float near = 1.2e-05f;
	using namespace cct;

	SCENARIO("Quaternion - construction")
	{
		GIVEN("A Quaternion(1, 2, 3, 4)")
		{
			Quaternionf quaternion(1, 2, 3, 4);
			THEN("Components are correct")
			{
				CHECK(quaternion.X() == 1);
				CHECK(quaternion.Y() == 2);
				CHECK(quaternion.Z() == 3);
				CHECK(quaternion.W() == 4);
			}
		}

		GIVEN("A Quaternion from EulerAngles(45, 25, 68)")
		{
			Quaternionf quaternion(EulerAnglesf(45, 25, 68));
			THEN("Components match expected values within tolerance")
			{
				CHECK_THAT(quaternion.X(), Catch::Matchers::WithinAbs(0.421557218f, near));
				CHECK_THAT(quaternion.Y(), Catch::Matchers::WithinAbs(0.374699116f, near));
				CHECK_THAT(quaternion.Z(), Catch::Matchers::WithinAbs(0.435713291f, near));
				CHECK_THAT(quaternion.W(), Catch::Matchers::WithinAbs(0.701458514f, near));
			}
		}
	}

	SCENARIO("Quaternion - arithmetic operators")
	{
		GIVEN("Two Quaternions(1, 2, 3, 4)")
		{
			Quaternionf q1(1, 2, 3, 4);
			Quaternionf q2(1, 2, 3, 4);

			WHEN("Added")
			{
				Quaternionf q3 = q1 + q2;
				THEN("Components are doubled")
				{
					CHECK(q3.X() == 2);
					CHECK(q3.Y() == 4);
					CHECK(q3.Z() == 6);
					CHECK(q3.W() == 8);
				}
			}

			WHEN("Subtracted")
			{
				Quaternionf q3 = q1 - q2;
				THEN("Components are zero")
				{
					CHECK(q3.X() == 0);
					CHECK(q3.Y() == 0);
					CHECK(q3.Z() == 0);
					CHECK(q3.W() == 0);
				}
			}
		}

		GIVEN("Two Quaternions from EulerAngles(10, 20, 30)")
		{
			EulerAnglesf eulerAngles(10, 20, 30);
			Quaternionf q1(eulerAngles);
			Quaternionf q2(eulerAngles);

			WHEN("Multiplied")
			{
				Quaternionf q3 = q1 * q2;
				THEN("Product matches expected values within tolerance")
				{
					CHECK_THAT(q3.X(), Catch::Matchers::WithinAbs(0.240985841f, near));
					CHECK_THAT(q3.Y(), Catch::Matchers::WithinAbs(0.357305080f, near));
					CHECK_THAT(q3.Z(), Catch::Matchers::WithinAbs(0.451658517f, near));
					CHECK_THAT(q3.W(), Catch::Matchers::WithinAbs(0.781193554f, near));
				}
			}
		}

		GIVEN("Two identical unit Quaternions")
		{
			Quaternionf q1(0.4431357f, 0.2080396f, -0.4884507f, 0.7223339f);
			Quaternionf q2(0.4431357f, 0.2080396f, -0.4884507f, 0.7223339f);

			WHEN("Divided")
			{
				Quaternionf q3 = q1 / q2;
				THEN("Result is identity (0, 0, 0, 1) within tolerance")
				{
					CHECK_THAT(q3.X(), Catch::Matchers::WithinAbs(0.f, near));
					CHECK_THAT(q3.Y(), Catch::Matchers::WithinAbs(0.f, near));
					CHECK_THAT(q3.Z(), Catch::Matchers::WithinAbs(0.f, near));
					CHECK_THAT(q3.W(), Catch::Matchers::WithinAbs(1.f, near));
				}
			}
		}
	}

	SCENARIO("Quaternion - scalar operators")
	{
		GIVEN("A Quaternion(1, 2, 3, 4)")
		{
			Quaternionf q1(1, 2, 3, 4);

			WHEN("Scalar 1 added")
			{
				Quaternionf q2 = q1 + 1;
				THEN("Components are incremented")
				{
					CHECK(q2.X() == 2);
					CHECK(q2.Y() == 3);
					CHECK(q2.Z() == 4);
					CHECK(q2.W() == 5);
				}
			}

			WHEN("Scalar 1 subtracted")
			{
				Quaternionf q2 = q1 - 1;
				THEN("Components are decremented")
				{
					CHECK(q2.X() == 0);
					CHECK(q2.Y() == 1);
					CHECK(q2.Z() == 2);
					CHECK(q2.W() == 3);
				}
			}

			WHEN("Multiplied by scalar 2")
			{
				Quaternionf q2 = q1 * 2;
				THEN("Components are doubled")
				{
					CHECK(q2.X() == 2);
					CHECK(q2.Y() == 4);
					CHECK(q2.Z() == 6);
					CHECK(q2.W() == 8);
				}
			}

			WHEN("Divided by scalar 2")
			{
				Quaternionf q2 = q1 / 2;
				THEN("Components are halved")
				{
					CHECK(q2.X() == 0.5);
					CHECK(q2.Y() == 1);
					CHECK(q2.Z() == 1.5);
					CHECK(q2.W() == 2);
				}
			}
		}
	}

	SCENARIO("Quaternion - compound assignment operators")
	{
		GIVEN("Two Quaternions(1, 2, 3, 4)")
		{
			Quaternionf q1(1, 2, 3, 4);
			Quaternionf q2(1, 2, 3, 4);

			WHEN("+=")
			{
				q1 += q2;
				THEN("Components are doubled")
				{
					CHECK(q1.X() == 2);
					CHECK(q1.Y() == 4);
					CHECK(q1.Z() == 6);
					CHECK(q1.W() == 8);
				}
			}

			WHEN("-=")
			{
				q1 -= q2;
				THEN("Components are zero")
				{
					CHECK(q1.X() == 0);
					CHECK(q1.Y() == 0);
					CHECK(q1.Z() == 0);
					CHECK(q1.W() == 0);
				}
			}
		}

		GIVEN("Two identical unit Quaternions")
		{
			Quaternionf q1(0.4431357f, 0.2080396f, -0.4884507f, 0.7223339f);
			Quaternionf q2(0.4431357f, 0.2080396f, -0.4884507f, 0.7223339f);

			WHEN("*=")
			{
				q1 *= q2;
				THEN("Product matches expected values within tolerance")
				{
					CHECK_THAT(q1.X(), Catch::Matchers::WithinAbs(0.6401839f, near));
					CHECK_THAT(q1.Y(), Catch::Matchers::WithinAbs(0.3005481f, near));
					CHECK_THAT(q1.Z(), Catch::Matchers::WithinAbs(-0.705649f, near));
					CHECK_THAT(q1.W(), Catch::Matchers::WithinAbs(0.04353243f, near));
				}
			}

			WHEN("/=")
			{
				q1 /= q2;
				THEN("Result is identity (0, 0, 0, 1) within tolerance")
				{
					CHECK_THAT(q1.X(), Catch::Matchers::WithinAbs(0.f, near));
					CHECK_THAT(q1.Y(), Catch::Matchers::WithinAbs(0.f, near));
					CHECK_THAT(q1.Z(), Catch::Matchers::WithinAbs(0.f, near));
					CHECK_THAT(q1.W(), Catch::Matchers::WithinAbs(1.f, near));
				}
			}
		}
	}

	SCENARIO("Quaternion - equality operators")
	{
		GIVEN("Two identical Quaternions(1, 2, 3, 4)")
		{
			Quaternionf q1(1, 2, 3, 4);
			Quaternionf q2(1, 2, 3, 4);
			THEN("They are equal") { CHECK(q1 == q2); }
		}

		GIVEN("Two Quaternions differing in W")
		{
			Quaternionf q1(1, 2, 3, 4);
			Quaternionf q2(1, 2, 3, 5);
			THEN("They are not equal") { CHECK(q1 != q2); }
		}
	}

	SCENARIO("Quaternion - vector multiplication")
	{
		GIVEN("An identity Quaternion (from zero EulerAngles)")
		{
			EulerAnglesf eulerAngles(0, 0, 0);
			Quaternionf q(eulerAngles);

			WHEN("Multiplied by Vector3f::Forward()")
			{
				Vector3f vec = q * Vector3f::Forward();
				THEN("The vector is unchanged") { CHECK(vec == Vector3f::Forward()); }
			}
		}
	}

	SCENARIO("Quaternion - Normalize")
	{
		GIVEN("A Quaternion from EulerAngles(45, 50, 78)")
		{
			EulerAnglesf eulerAngles(45, 50, 78);
			Quaternionf q(eulerAngles);

			WHEN("Normalized")
			{
				q.Normalize();
				THEN("Length is 1 within tolerance") { CHECK_THAT(q.Length(), Catch::Matchers::WithinAbs(1.f, near)); }
			}
		}
	}

	SCENARIO("Quaternion - ToEulerAngles round-trip")
	{
		GIVEN("An identity Quaternion (from zero angles)")
		{
			Quaternionf q1(EulerAnglesf(0, 0, 0));
			WHEN("Converted to EulerAngles")
			{
				EulerAnglesf angles = q1.ToEulerAngles();
				THEN("All angles are ~0")
				{
					CHECK_THAT(angles.Pitch(), Catch::Matchers::WithinAbs(0.f, near));
					CHECK_THAT(angles.Yaw(),   Catch::Matchers::WithinAbs(0.f, near));
					CHECK_THAT(angles.Roll(),  Catch::Matchers::WithinAbs(0.f, near));
				}
			}
		}

		GIVEN("A Quaternion from EulerAngles(30, 25, 68)")
		{
			Quaternionf q2(EulerAnglesf(30, 25, 68));
			WHEN("Converted to EulerAngles")
			{
				EulerAnglesf angles = q2.ToEulerAngles();
				THEN("Angles match within tolerance")
				{
					CHECK_THAT(angles.Pitch(), Catch::Matchers::WithinAbs(30.f, near));
					CHECK_THAT(angles.Yaw(),   Catch::Matchers::WithinAbs(25.f, near));
					CHECK_THAT(angles.Roll(),  Catch::Matchers::WithinAbs(68.f, near));
				}
			}
		}
	}

	SCENARIO("Quaternion - ToRotationMatrix")
	{
		GIVEN("A specific unit Quaternion")
		{
			// results calculated from https://www.andre-gaschler.com/rotationconverter/
			// Angles are expressed in radians
			auto matrix1 = Quaternionf(-0.122257f, 0.4018732f, -0.9020968f, -0.0988561f).ToRotationMatrix<Matrix4f>();
			constexpr Matrix4f expected(
				-0.9505614f, -0.2766193f, 0.1411200f, 0.f,
				0.0800920f, -0.6574507f, -0.7492288f, 0.f,
				0.3000306f, -0.7008854f, 0.6471023f, 0.f,
				0.f, 0.f, 0.f, 1.f
			);

			THEN("All matrix elements match within tolerance")
			{
				const auto* matData = matrix1.Data();
				const auto* expectData = expected.Data();
				for (std::size_t i = 0; i < matrix1.GetSize(); ++i)
				{
					CHECK_THAT(matData[i], Catch::Matchers::WithinAbs(expectData[i], near));
				}
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
