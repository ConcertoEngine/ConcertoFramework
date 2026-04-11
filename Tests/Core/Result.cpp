//
// Created by arthur on 09/12/2024.
//

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <Concerto/Core/Types/Types.hpp>
#include <Concerto/Core/Result/Result.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	class Bar
	{
	public:
		Bar() : a(0), b(false), c(0) {}
		Bar(Int32 aa, bool bb, Int32 cc) : a(aa), b(bb), c(cc) {}

		Int32 a;
		bool b;
		Int32 c;
	};

	SCENARIO("Result")
	{
		GIVEN("A Result<Int32, string> initialized with value 28")
		{
			Result<Int32, std::string> result(28);
			THEN("It is Ok and has the correct value")
			{
				REQUIRE(result.IsOk());
				REQUIRE_FALSE(result.IsError());
				REQUIRE(result.GetValue() == 28);
			}
		}

		GIVEN("A Result<Int32, string> initialized with error \"Foo\"")
		{
			Result<Int32, std::string> result(std::string("Foo"));
			THEN("It is an error and has the correct message")
			{
				REQUIRE_FALSE(result.IsOk());
				REQUIRE(result.IsError());
				REQUIRE(result.GetError() == "Foo");
			}
		}

		GIVEN("A Result<Bar, string> initialized with variadic value args (1, true, 2)")
		{
			Result<Bar, std::string> result(std::in_place_type_t<Bar>(), 1, true, 2);
			THEN("It is Ok and the Bar fields are correct")
			{
				REQUIRE(result.IsOk());
				REQUIRE_FALSE(result.IsError());
				auto value = result.GetValue();
				REQUIRE(value.a == 1);
				REQUIRE(value.b == true);
				REQUIRE(value.c == 2);
			}
		}

		GIVEN("A Result<string, Bar> initialized with variadic error args (1, true, 2)")
		{
			Result<std::string, Bar> result(std::in_place_type_t<Bar>(), 1, true, 2);
			THEN("It is an error and the Bar fields are correct")
			{
				REQUIRE_FALSE(result.IsOk());
				REQUIRE(result.IsError());
				auto value = result.GetError();
				REQUIRE(value.a == 1);
				REQUIRE(value.b == true);
				REQUIRE(value.c == 2);
			}
		}

		GIVEN("A Result<void, Bar> initialized with variadic error args (1, true, 2)")
		{
			Result<void, Bar> result(std::in_place_type_t<Bar>(), 1, true, 2);
			THEN("It is an error and the Bar fields are correct")
			{
				REQUIRE_FALSE(result.IsOk());
				REQUIRE(result.IsError());
				auto value = result.GetError();
				REQUIRE(value.a == 1);
				REQUIRE(value.b == true);
				REQUIRE(value.c == 2);
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
