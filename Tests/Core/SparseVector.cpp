//
// Created by arthur on 12/10/2023.
//

#include <memory>
#include <catch2/catch_test_macros.hpp>
#include <Concerto/Core/SparseVector/SparseVector.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	SCENARIO("SparseVector - emplace and access")
	{
		GIVEN("A SparseVector<int> with values at indices 5 and 100")
		{
			SparseVector<int> sparseVector;
			sparseVector.Emplace(5, 999);
			sparseVector.Emplace(100, 999);

			THEN("Has() is correct and operator[] returns the right values")
			{
				REQUIRE(sparseVector.Has(5));
				REQUIRE(sparseVector.Has(100));
				REQUIRE_FALSE(sparseVector.Has(0));
				REQUIRE_FALSE(sparseVector.Has(102));
				REQUIRE(sparseVector[5] == 999);
				REQUIRE(sparseVector[100] == 999);
			}

			THEN("Accessing missing indices throws the correct exceptions")
			{
				REQUIRE_THROWS_AS(sparseVector[0], std::runtime_error);
				REQUIRE_THROWS_AS(sparseVector[102], std::out_of_range);
			}
		}

		GIVEN("A SparseVector<HardToCopy>")
		{
			using Ptr = std::unique_ptr<int>;
			class HardToCopy
			{
			public:
				HardToCopy() = default;
				HardToCopy(Ptr a, int b, int c) : _ptr(std::move(a)), _b(b), _c(c) {}
				HardToCopy(const HardToCopy&) = delete;
				HardToCopy(HardToCopy&&) = default;
				~HardToCopy() = default;
				HardToCopy& operator=(const HardToCopy&) = delete;
				HardToCopy& operator=(HardToCopy&&) = default;
			private:
				Ptr _ptr;
				int _b;
				int _c;
			};

			SparseVector<HardToCopy> sparseVector;

			WHEN("Emplace with a default element and then with moved args")
			{
				sparseVector.Emplace(0, HardToCopy());
				auto ptr = std::make_unique<int>(5);
				sparseVector.Emplace(1, std::move(ptr), 2, 3);
				THEN("No crash and elements are inserted") { CHECK(sparseVector.Has(0)); }
			}
		}
	}

	SCENARIO("SparseVector - default construction")
	{
		GIVEN("A default-constructed SparseVector<int>")
		{
			SparseVector<int> sparseVector;
			THEN("No elements are present")
			{
				REQUIRE_FALSE(sparseVector.Has(0));
				REQUIRE_FALSE(sparseVector.Has(1));
				REQUIRE_FALSE(sparseVector.Has(2));
			}
		}
	}

	SCENARIO("SparseVector - copy and move semantics")
	{
		GIVEN("A SparseVector<int> with values at 0, 1, 2")
		{
			SparseVector<int> sparseVector1;
			sparseVector1.Emplace(0, 1);
			sparseVector1.Emplace(1, 2);
			sparseVector1.Emplace(2, 3);

			WHEN("Copy constructed")
			{
				SparseVector<int> sparseVector2(sparseVector1);
				THEN("The copy has the same elements")
				{
					REQUIRE(sparseVector2.Has(0));
					REQUIRE(sparseVector2.Has(1));
					REQUIRE(sparseVector2.Has(2));
					REQUIRE(sparseVector2[0] == 1);
					REQUIRE(sparseVector2[1] == 2);
					REQUIRE(sparseVector2[2] == 3);
				}
			}

			WHEN("Move constructed")
			{
				SparseVector<int> sparseVector2(std::move(sparseVector1));
				THEN("The moved-into vector has the elements")
				{
					REQUIRE(sparseVector2.Has(0));
					REQUIRE(sparseVector2.Has(1));
					REQUIRE(sparseVector2.Has(2));
					REQUIRE(sparseVector2[0] == 1);
					REQUIRE(sparseVector2[1] == 2);
					REQUIRE(sparseVector2[2] == 3);
				}
			}

			WHEN("Copy assigned")
			{
				SparseVector<int> sparseVector2;
				sparseVector2 = sparseVector1;
				THEN("The assigned vector has the elements")
				{
					REQUIRE(sparseVector2.Has(0));
					REQUIRE(sparseVector2.Has(1));
					REQUIRE(sparseVector2.Has(2));
					REQUIRE(sparseVector2[0] == 1);
					REQUIRE(sparseVector2[1] == 2);
					REQUIRE(sparseVector2[2] == 3);
				}
			}

			WHEN("Move assigned")
			{
				SparseVector<int> sparseVector2;
				sparseVector2 = std::move(sparseVector1);
				THEN("The assigned vector has the elements")
				{
					REQUIRE(sparseVector2.Has(0));
					REQUIRE(sparseVector2.Has(1));
					REQUIRE(sparseVector2.Has(2));
					REQUIRE(sparseVector2[0] == 1);
					REQUIRE(sparseVector2[1] == 2);
					REQUIRE(sparseVector2[2] == 3);
				}
			}
		}
	}

	SCENARIO("SparseVector - Clear and Erase")
	{
		GIVEN("A SparseVector<int> with values at 0, 1, 2")
		{
			SparseVector<int> sparseVector;
			sparseVector.Emplace(0, 1);
			sparseVector.Emplace(1, 2);
			sparseVector.Emplace(2, 3);

			WHEN("Clear() is called")
			{
				sparseVector.Clear();
				THEN("No elements remain")
				{
					REQUIRE_FALSE(sparseVector.Has(0));
					REQUIRE_FALSE(sparseVector.Has(1));
					REQUIRE_FALSE(sparseVector.Has(2));
				}
			}

			WHEN("Erase(1) is called")
			{
				sparseVector.Erase(1);
				THEN("Only index 1 is removed")
				{
					REQUIRE(sparseVector.Has(0));
					REQUIRE_FALSE(sparseVector.Has(1));
					REQUIRE(sparseVector.Has(2));
					REQUIRE(sparseVector[0] == 1);
					REQUIRE(sparseVector[2] == 3);
				}
			}
		}
	}

	SCENARIO("SparseVector - iteration")
	{
		GIVEN("A SparseVector<int> with sequential values 1, 2, 3")
		{
			SparseVector<int> sparseVector;
			sparseVector.Emplace(0, 1);
			sparseVector.Emplace(1, 2);
			sparseVector.Emplace(2, 3);

			WHEN("Iterated with range-for (non-const)")
			{
				int i = 0;
				for (auto& value : sparseVector)
				{
					REQUIRE(value == i + 1);
					i++;
				}
				THEN("All three values were visited in order") { CHECK(i == 3); }
			}

			WHEN("Iterated with range-for (const)")
			{
				int i = 0;
				for (const auto& value : sparseVector)
				{
					REQUIRE(value == i + 1);
					i++;
				}
				THEN("All three values were visited in order") { CHECK(i == 3); }
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
