//
// Created by arthur on 23/02/2023.
//

#include <catch2/catch_test_macros.hpp>

#include "Concerto/Core/DeferredExit/DeferredExit.hpp"
#include "Concerto/Core/Types/Types.hpp"

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	SCENARIO("DeferredExit")
	{
		GIVEN("A DeferredExit with a callback that sets i = 42")
		{
			Int32 i = 0;
			{
				DeferredExit _([&]()
				{
					i = 42;
				});
			}
			THEN("The callback is invoked when the DeferredExit goes out of scope") { REQUIRE(i == 42); }
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
