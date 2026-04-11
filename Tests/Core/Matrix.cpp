//
// Created by arthur on 30/08/2022.
//

#include <catch2/catch_test_macros.hpp>

#include "Concerto/Core/Math/Matrix/Matrix.hpp"

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	SCENARIO("Matrix - construction and dimensions")
	{
		GIVEN("A default-constructed 3x3 float matrix")
		{
			Matrix<float, 3, 3> m{};
			THEN("Width and Height are 3")
			{
				REQUIRE(m.GetWidth() == 3);
				REQUIRE(m.GetHeight() == 3);
			}
		}
	}

	SCENARIO("Matrix - element access")
	{
		GIVEN("A 3x3 matrix with values 1..9")
		{
			Matrix<float, 3, 3> m(1.f, 2.f, 3.f,
								  4.f, 5.f, 6.f,
								  7.f, 8.f, 9.f);
			THEN("GetElement returns the correct values per row/column")
			{
				REQUIRE(m.GetElement(0, 0) == 1.f);
				REQUIRE(m.GetElement(0, 1) == 2.f);
				REQUIRE(m.GetElement(0, 2) == 3.f);
				REQUIRE(m.GetElement(1, 0) == 4.f);
				REQUIRE(m.GetElement(1, 1) == 5.f);
				REQUIRE(m.GetElement(1, 2) == 6.f);
				REQUIRE(m.GetElement(2, 0) == 7.f);
				REQUIRE(m.GetElement(2, 1) == 8.f);
				REQUIRE(m.GetElement(2, 2) == 9.f);
			}
		}
	}

	SCENARIO("Matrix - matrix arithmetic operators")
	{
		GIVEN("Two identical 3x3 matrices with values 1..9")
		{
			Matrix<float, 3, 3> m1(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
			Matrix<float, 3, 3> m2(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);

			WHEN("Added (operator+)")
			{
				Matrix<float, 3, 3> m3 = m1 + m2;
				THEN("Each element is doubled")
				{
					REQUIRE(m3(0, 0) == 2.f);   REQUIRE(m3(0, 1) == 4.f);   REQUIRE(m3(0, 2) == 6.f);
					REQUIRE(m3(1, 0) == 8.f);   REQUIRE(m3(1, 1) == 10.f);  REQUIRE(m3(1, 2) == 12.f);
					REQUIRE(m3(2, 0) == 14.f);  REQUIRE(m3(2, 1) == 16.f);  REQUIRE(m3(2, 2) == 18.f);
				}
			}

			WHEN("Subtracted (operator-)")
			{
				Matrix<float, 3, 3> m3 = m1 - m2;
				THEN("All elements are zero")
				{
					for (int r = 0; r < 3; ++r)
						for (int c = 0; c < 3; ++c)
							REQUIRE(m3(r, c) == 0.f);
				}
			}

			WHEN("Multiplied (operator*)")
			{
				Matrix<float, 3, 3> m3 = m1 * m2;
				THEN("Result is the matrix product")
				{
					REQUIRE(m3(0, 0) == 30.f);  REQUIRE(m3(0, 1) == 36.f);  REQUIRE(m3(0, 2) == 42.f);
					REQUIRE(m3(1, 0) == 66.f);  REQUIRE(m3(1, 1) == 81.f);  REQUIRE(m3(1, 2) == 96.f);
					REQUIRE(m3(2, 0) == 102.f); REQUIRE(m3(2, 1) == 126.f); REQUIRE(m3(2, 2) == 150.f);
				}
			}

			WHEN("Divided (operator/)")
			{
				Matrix<float, 3, 3> m3 = m1 / m2;
				THEN("All elements are 1")
				{
					for (int r = 0; r < 3; ++r)
						for (int c = 0; c < 3; ++c)
							REQUIRE(m3(r, c) == 1.f);
				}
			}
		}
	}

	SCENARIO("Matrix - scalar arithmetic operators")
	{
		GIVEN("A 3x3 matrix with values 1..9")
		{
			Matrix<float, 3, 3> m1(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);

			WHEN("Scalar 1 added")
			{
				Matrix<float, 3, 3> m2 = m1 + 1.f;
				THEN("Each element is incremented by 1")
				{
					REQUIRE(m2(0, 0) == 2.f);  REQUIRE(m2(0, 1) == 3.f);  REQUIRE(m2(0, 2) == 4.f);
					REQUIRE(m2(1, 0) == 5.f);  REQUIRE(m2(1, 1) == 6.f);  REQUIRE(m2(1, 2) == 7.f);
					REQUIRE(m2(2, 0) == 8.f);  REQUIRE(m2(2, 1) == 9.f);  REQUIRE(m2(2, 2) == 10.f);
				}
			}

			WHEN("Scalar 1 subtracted")
			{
				Matrix<float, 3, 3> m2 = m1 - 1.f;
				THEN("Each element is decremented by 1")
				{
					REQUIRE(m2(0, 0) == 0.f);  REQUIRE(m2(0, 1) == 1.f);  REQUIRE(m2(0, 2) == 2.f);
					REQUIRE(m2(1, 0) == 3.f);  REQUIRE(m2(1, 1) == 4.f);  REQUIRE(m2(1, 2) == 5.f);
					REQUIRE(m2(2, 0) == 6.f);  REQUIRE(m2(2, 1) == 7.f);  REQUIRE(m2(2, 2) == 8.f);
				}
			}

			WHEN("Multiplied by scalar 2")
			{
				Matrix<float, 3, 3> m2 = m1 * 2.f;
				THEN("Each element is doubled")
				{
					REQUIRE(m2(0, 0) == 2.f);  REQUIRE(m2(0, 1) == 4.f);  REQUIRE(m2(0, 2) == 6.f);
					REQUIRE(m2(1, 0) == 8.f);  REQUIRE(m2(1, 1) == 10.f); REQUIRE(m2(1, 2) == 12.f);
					REQUIRE(m2(2, 0) == 14.f); REQUIRE(m2(2, 1) == 16.f); REQUIRE(m2(2, 2) == 18.f);
				}
			}

			WHEN("Divided by scalar 2")
			{
				Matrix<float, 3, 3> m2 = m1 / 2.f;
				THEN("Each element is halved")
				{
					REQUIRE(m2(0, 0) == 0.5f); REQUIRE(m2(0, 1) == 1.f);  REQUIRE(m2(0, 2) == 1.5f);
					REQUIRE(m2(1, 0) == 2.f);  REQUIRE(m2(1, 1) == 2.5f); REQUIRE(m2(1, 2) == 3.f);
					REQUIRE(m2(2, 0) == 3.5f); REQUIRE(m2(2, 1) == 4.f);  REQUIRE(m2(2, 2) == 4.5f);
				}
			}
		}
	}

	SCENARIO("Matrix - compound assignment operators (matrix)")
	{
		GIVEN("Two identical 3x3 matrices with values 1..9")
		{
			Matrix<float, 3, 3> m1(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
			Matrix<float, 3, 3> m2(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);

			WHEN("+=")
			{
				m1 += m2;
				THEN("m1 elements are doubled")
				{
					REQUIRE(m1(0, 0) == 2.f);  REQUIRE(m1(0, 1) == 4.f);  REQUIRE(m1(0, 2) == 6.f);
					REQUIRE(m1(1, 0) == 8.f);  REQUIRE(m1(1, 1) == 10.f); REQUIRE(m1(1, 2) == 12.f);
					REQUIRE(m1(2, 0) == 14.f); REQUIRE(m1(2, 1) == 16.f); REQUIRE(m1(2, 2) == 18.f);
				}
			}

			WHEN("-=")
			{
				m1 -= m2;
				THEN("m1 elements are all zero")
				{
					for (int r = 0; r < 3; ++r)
						for (int c = 0; c < 3; ++c)
							REQUIRE(m1(r, c) == 0.f);
				}
			}

			WHEN("*=")
			{
				m1 *= m2;
				THEN("m1 elements are squared")
				{
					REQUIRE(m1(0, 0) == 1.f);  REQUIRE(m1(0, 1) == 4.f);  REQUIRE(m1(0, 2) == 9.f);
					REQUIRE(m1(1, 0) == 16.f); REQUIRE(m1(1, 1) == 25.f); REQUIRE(m1(1, 2) == 36.f);
					REQUIRE(m1(2, 0) == 49.f); REQUIRE(m1(2, 1) == 64.f); REQUIRE(m1(2, 2) == 81.f);
				}
			}

			WHEN("/=")
			{
				m1 /= m2;
				THEN("m1 elements are all 1")
				{
					for (int r = 0; r < 3; ++r)
						for (int c = 0; c < 3; ++c)
							REQUIRE(m1(r, c) == 1.f);
				}
			}
		}
	}

	SCENARIO("Matrix - compound assignment operators (scalar)")
	{
		GIVEN("A 3x3 matrix with values 1..9")
		{
			Matrix<float, 3, 3> m1(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);

			WHEN("+= scalar 1")
			{
				m1 += 1.f;
				THEN("Each element is incremented by 1")
				{
					REQUIRE(m1(0, 0) == 2.f);  REQUIRE(m1(0, 1) == 3.f);  REQUIRE(m1(0, 2) == 4.f);
					REQUIRE(m1(1, 0) == 5.f);  REQUIRE(m1(1, 1) == 6.f);  REQUIRE(m1(1, 2) == 7.f);
					REQUIRE(m1(2, 0) == 8.f);  REQUIRE(m1(2, 1) == 9.f);  REQUIRE(m1(2, 2) == 10.f);
				}
			}

			WHEN("-= scalar 1")
			{
				m1 -= 1.f;
				THEN("Each element is decremented by 1")
				{
					REQUIRE(m1(0, 0) == 0.f);  REQUIRE(m1(0, 1) == 1.f);  REQUIRE(m1(0, 2) == 2.f);
					REQUIRE(m1(1, 0) == 3.f);  REQUIRE(m1(1, 1) == 4.f);  REQUIRE(m1(1, 2) == 5.f);
					REQUIRE(m1(2, 0) == 6.f);  REQUIRE(m1(2, 1) == 7.f);  REQUIRE(m1(2, 2) == 8.f);
				}
			}

			WHEN("*= scalar 2")
			{
				m1 *= 2.f;
				THEN("Each element is doubled")
				{
					REQUIRE(m1(0, 0) == 2.f);  REQUIRE(m1(0, 1) == 4.f);  REQUIRE(m1(0, 2) == 6.f);
					REQUIRE(m1(1, 0) == 8.f);  REQUIRE(m1(1, 1) == 10.f); REQUIRE(m1(1, 2) == 12.f);
					REQUIRE(m1(2, 0) == 14.f); REQUIRE(m1(2, 1) == 16.f); REQUIRE(m1(2, 2) == 18.f);
				}
			}

			WHEN("/= scalar 2")
			{
				m1 /= 2.f;
				THEN("Each element is halved")
				{
					REQUIRE(m1(0, 0) == 0.5f); REQUIRE(m1(0, 1) == 1.f);  REQUIRE(m1(0, 2) == 1.5f);
					REQUIRE(m1(1, 0) == 2.f);  REQUIRE(m1(1, 1) == 2.5f); REQUIRE(m1(1, 2) == 3.f);
					REQUIRE(m1(2, 0) == 3.5f); REQUIRE(m1(2, 1) == 4.f);  REQUIRE(m1(2, 2) == 4.5f);
				}
			}
		}
	}

	//SCENARIO("Matrix - Translate") { ... }  // commented out in original
} // namespace CCT_ANONYMOUS_NAMESPACE
