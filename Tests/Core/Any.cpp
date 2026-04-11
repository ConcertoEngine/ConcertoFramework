//
// Created by Arthur on 08/10/2025.
//

#include <catch2/catch_test_macros.hpp>

#include <Concerto/Core/Any/Any.hpp>
#include <Concerto/Core/TypeInfo/TypeInfo.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
    using namespace cct;

    struct Big
    {
        Big() = default;
        explicit Big(int v) : x(v) {}
        int x = 0;
        std::array<char, 64> buf{}; // ensure > small buffer
    };

    SCENARIO("Any")
    {
        GIVEN("An Any holding a small value (int 42)")
        {
            auto a = Any::Make<int>(42);

            THEN("It has a value, is of type int, and returns the correct value")
            {
                REQUIRE(a.HasValue());
                REQUIRE(a.Is<int>());
                REQUIRE_FALSE(a.Is<float>());
                CHECK(a.As<int>() == 42);
            }

            WHEN("Reset() is called")
            {
                a.Reset();
                THEN("It has no value") { CHECK_FALSE(a.HasValue()); }
            }
        }

        GIVEN("An Any holding a large value (Big with x=99)")
        {
            Big b(99);
            auto a = Any::Make<Big>(b);

            THEN("It has a value, is of type Big, and x is 99")
            {
                REQUIRE(a.HasValue());
                REQUIRE(a.Is<Big>());
                CHECK(a.As<Big>().x == 99);
            }
        }

        GIVEN("An Any holding a pointer to int")
        {
            int v = 5;
            auto a = Any::Make<int*>(&v);

            THEN("It has a value and is of type int*")
            {
                REQUIRE(a.HasValue());
                REQUIRE(a.Is<int*>());
            }

            WHEN("The pointer is dereferenced and modified")
            {
                int* p = a.As<int*>();
                *p = 11;
                THEN("The original value is modified") { CHECK(v == 11); }
            }
        }

        GIVEN("An Any holding a reference to int")
        {
            int v = 7;
            auto a = Any::Make<int&>(v);

            THEN("It has a value and is of type int&")
            {
                REQUIRE(a.HasValue());
                REQUIRE(a.Is<int&>());
            }

            WHEN("The reference is modified")
            {
                int& r = a.As<int&>();
                r = 15;
                THEN("The original value is modified") { CHECK(v == 15); }
            }
        }

        GIVEN("An Any holding an int")
        {
            auto a = Any::Make<int>(3);
            REQUIRE(a.Is<int>());

            WHEN("Cast to float")
            {
                THEN("It throws std::bad_cast") { CHECK_THROWS_AS((void)a.As<float>(), std::bad_cast); }
            }
        }
    }
} // namespace CCT_ANONYMOUS_NAMESPACE
