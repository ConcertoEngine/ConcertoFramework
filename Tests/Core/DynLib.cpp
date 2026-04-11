//
// Created by arthur on 02/06/2024.
//

#include <catch2/catch_test_macros.hpp>
#include <Concerto/Core/DynLib/DynLib.hpp>

#ifdef CCT_PLATFORM_POSIX
#define PREFIX "lib"
#else
#define PREFIX ""
#endif

namespace CCT_ANONYMOUS_NAMESPACE
{
	SCENARIO("DynLib - Invoke")
	{
		GIVEN("A DynLib loaded with the dummy library")
		{
			cct::DynLib lib;
			REQUIRE(lib.Load(PREFIX "concerto-core-dummy"));

			WHEN("Dummy() is invoked")
			{
				lib.Invoke<void>("Dummy");
				const cct::FunctionRef func = lib.GetFunction<void>("Dummy");
				THEN("The function handle is valid") { CHECK(func.operator bool() == true); }
			}

			WHEN("DummyInt() is invoked")
			{
				THEN("It returns 42") { CHECK(lib.Invoke<int>("DummyInt") == 42); }
			}

			WHEN("GlobalInt is retrieved")
			{
				const int* globalValue = lib.GetValue<int>("GlobalInt");
				THEN("The pointer is valid and the value is 42")
				{
					REQUIRE(globalValue != nullptr);
					CHECK(*globalValue == 42);
				}
			}

			WHEN("A non-existing value is retrieved")
			{
				const int* notExisting = lib.GetValue<int>("NotExisting");
				THEN("It returns nullptr") { CHECK(notExisting == nullptr); }
			}

			WHEN("Increment(5) is invoked")
			{
				THEN("It returns 6") { CHECK(lib.Invoke<int>("Increment", 5) == 6); }
			}

			WHEN("A non-existing function is invoked")
			{
				THEN("It throws std::runtime_error") { CHECK_THROWS_AS(lib.Invoke<int>("NotExisting", 5), std::runtime_error); }
			}

			WHEN("The library is unloaded")
			{
				bool res = lib.Unload();
				THEN("Unload returns true") { CHECK(res == true); }
			}
		}
	}

	SCENARIO("DynLib - Loading")
	{
		GIVEN("An empty DynLib")
		{
			cct::DynLib lib;

			THEN("Unload on an empty library returns false") { CHECK_FALSE(lib.Unload()); }

			WHEN("A non-existing library is loaded")
			{
				bool res = lib.Load(PREFIX "not-exist");
				THEN("Load returns false and GetSymbol returns nullptr")
				{
					CHECK_FALSE(res);
					CHECK(lib.GetSymbol("foo") == nullptr);
				}
			}

			WHEN("The dummy library is loaded")
			{
				bool res = lib.Load(PREFIX "concerto-core-dummy");
				THEN("Load returns true") { CHECK(res); }
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
