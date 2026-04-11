//
// Created by arthur on 10/08/2023.
//

#include <catch2/catch_test_macros.hpp>
#include <Concerto/Core/Math/Transform/Transform.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;

	SCENARIO("Serialize - Transform round-trip")
	{
		GIVEN("A Transform with non-trivial location, rotation, and scale")
		{
			Vector3f location(10.f, 15.f, 20.f);
			Quaternion rotation = EulerAngles(20.f, 30.f, 40.f).ToQuaternion();
			Vector3f scale(10.f, 20.f, 30.f);
			Transform transform(location, rotation, scale);

			WHEN("Serialized to a Stream then deserialized")
			{
				Stream stream;
				transform.Serialize(stream);

				stream.SetCursorPos(0);
				Transform result;
				result.Deserialize(stream);

				THEN("The deserialized Transform equals the original") { REQUIRE(transform == result); }
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
