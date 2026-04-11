//
// Created by arthur on 03/06/2024.
//

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <Concerto/Core/FunctionRef/FunctionRef.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
	void foo(int& value) { value = 42; }

	SCENARIO("FunctionRef")
	{
		GIVEN("An empty FunctionRef<void()>")
		{
			cct::FunctionRef<void()> func;

			THEN("It evaluates to false") { REQUIRE(func.operator bool() == false); }

			WHEN("A lambda is assigned")
			{
				auto lambda = []() {};
				func = lambda;
				THEN("It evaluates to true") { REQUIRE(func.operator bool() == true); }
			}
		}

		GIVEN("A FunctionRef<void()> holding a capturing lambda")
		{
			int value = 0;
			auto lambda = [&value]() { value = 42; };
			cct::FunctionRef<void()> func = lambda;

			WHEN("The function is called")
			{
				func();
				THEN("The captured value is set to 42") { REQUIRE(value == 42); }
			}
		}

		GIVEN("A FunctionRef<void(int&)> holding a free function")
		{
			cct::FunctionRef<void(int&)> func = foo;

			WHEN("The function is called with int 0")
			{
				int value = 0;
				func(value);
				THEN("The value is set to 42") { REQUIRE(value == 42); }
			}
		}

		GIVEN("A FunctionRef holding a lambda with multiple arguments")
		{
			auto lambda = [](std::size_t& value, const std::string& str) { value = str.size(); };
			cct::FunctionRef<void(std::size_t&, const std::string&)> func = lambda;

			WHEN("Called with a string of length 5")
			{
				std::size_t value = 0;
				std::string str = "Hello";
				func(value, str);
				THEN("value equals the string size") { REQUIRE(value == str.size()); }
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
