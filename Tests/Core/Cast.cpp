//
// Created by arthur on 08/08/2024.
//

#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include <Concerto/Core/Cast.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	constexpr std::string_view BaseStr = "Base";
	constexpr std::string_view DerivedStr = "Derived";

	class Base {
	public:
		virtual ~Base() = default;
		virtual std::string_view Speak() const {
			return BaseStr;
		}
	};

	class Derived : public Base {
	public:
		std::string_view Speak() const override {
			return DerivedStr;
		}
	};

	SCENARIO("Cast")
	{
		GIVEN("A Derived object referenced as Base&")
		{
			Derived derived;
			Base& base = derived;

			WHEN("Cast to Derived&")
			{
				Derived& casted = Cast<Derived&>(base);
				THEN("The cast succeeds and Speak() returns DerivedStr") { CHECK(casted.Speak() == DerivedStr); }
			}
		}

		GIVEN("A Derived object as Base&&")
		{
			Derived derived;
			Base&& base = std::move(derived);

			WHEN("Cast to Derived&&")
			{
				Derived&& casted = Cast<Derived&&>(std::move(base));
				THEN("The cast succeeds") { CHECK(casted.Speak() == DerivedStr); }
			}
		}

		GIVEN("A const Derived object as const Base&")
		{
			const Derived derived;
			const Base& base = derived;

			WHEN("Cast to const Derived&")
			{
				auto& casted = Cast<const Derived&>(base);
				THEN("The cast succeeds") { CHECK(casted.Speak() == DerivedStr); }
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
