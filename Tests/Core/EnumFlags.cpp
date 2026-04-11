//
// Created by Arthur on 06/10/2025.
//

#include <catch2/catch_test_macros.hpp>

#include <Concerto/Core/EnumFlags/EnumFlags.hpp>

enum class MyFlags : unsigned
{
	None = 0u,
	A = 1u << 0,
	B = 1u << 1,
	C = 1u << 2,
};
CCT_ENABLE_ENUM_FLAGS(MyFlags)

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	SCENARIO("EnumFlags - basic OR, AND and Contains")
	{
		GIVEN("EnumFlags with A and B set")
		{
			auto f = MyFlags::A | MyFlags::B;

			THEN("Contains A and B, but not C")
			{
				CHECK(f.Contains(MyFlags::A));
				CHECK(f.Contains(MyFlags::B));
				CHECK_FALSE(f.Contains(MyFlags::C));
			}

			THEN("Is truthy, while empty flags are falsy")
			{
				CHECK(static_cast<bool>(f));
				CHECK_FALSE(static_cast<bool>(EnumFlags<MyFlags>{}));
			}

			WHEN("ANDed with A")
			{
				auto g = f & MyFlags::A;
				THEN("Result contains only A")
				{
					CHECK(g.Contains(MyFlags::A));
					CHECK_FALSE(g.Contains(MyFlags::B));
					CHECK_FALSE(g.Contains(MyFlags::C));
				}
			}
		}
	}

	SCENARIO("EnumFlags - XOR, Toggle, Set, Reset")
	{
		GIVEN("EnumFlags with only A set")
		{
			EnumFlags<MyFlags> f = MyFlags::A;

			WHEN("XOR with B, Toggle A, Set C, then Reset B")
			{
				f ^= MyFlags::B;
				CHECK(f.Contains(MyFlags::B));
				f.Toggle(MyFlags::A);
				CHECK_FALSE(f.Contains(MyFlags::A));
				f.Set(MyFlags::C);
				CHECK(f.Contains(MyFlags::C));
				f.Reset(MyFlags::B);
				THEN("B is no longer set") { CHECK_FALSE(f.Contains(MyFlags::B)); }
			}
		}
	}

	SCENARIO("EnumFlags - unary NOT and Clear")
	{
		GIVEN("EnumFlags with A and B set")
		{
			EnumFlags<MyFlags> f = MyFlags::A | MyFlags::B;

			WHEN("NOT A is masked with A")
			{
				auto notA = ~EnumFlags<MyFlags>(MyFlags::A);
				auto masked = notA & MyFlags::A;
				THEN("The masked result has no bits set") { CHECK(masked.None()); }
			}

			WHEN("Clear() is called")
			{
				f.Clear();
				THEN("None() is true and Any() is false")
				{
					CHECK(f.None());
					CHECK_FALSE(f.Any());
				}
			}
		}
	}

	SCENARIO("EnumFlags - raw underlying value")
	{
		GIVEN("EnumFlags with A and C set")
		{
			EnumFlags<MyFlags> f = MyFlags::A | MyFlags::C;

			THEN("Value() equals the bitwise OR of A and C as underlying type")
			{
				using U = EnumFlags<MyFlags>::Underlying;
				CHECK(f.Value() == static_cast<U>(static_cast<U>(MyFlags::A) | static_cast<U>(MyFlags::C)));
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
