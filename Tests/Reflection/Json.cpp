#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/Json/Json.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>

#include "SampleBar.refl.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using Catch::Matchers::ContainsSubstring;
using Catch::Matchers::WithinAbs;

namespace
{
	// Same pattern as Vector.cpp — each SCENARIO creates its own PackageLoader,
	// scoped to the GIVEN block. The loader must outlive every reflected access.
	void LoadPackages(cct::refl::PackageLoader& loader)
	{
		REQUIRE(loader.AddPackage(CreateConcertoReflectionPackage()));
		REQUIRE(loader.AddPackage(CreateConcertoReflectionTestsPackage()));
		loader.LoadPackages();
	}

	std::unique_ptr<cct::sample::JsonSample> MakeSample()
	{
		auto obj = cct::sample::JsonSample::GetClass()
					   ->CreateDefaultObject<cct::sample::JsonSample>();
		REQUIRE(obj != nullptr);
		return obj;
	}
} // namespace

SCENARIO("Json::ToJson - serializes native primitive members")
{
	GIVEN("A JsonSample populated with mixed primitive values")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto sample = MakeSample();
		sample->m_intValue = 42;
		sample->m_floatValue = 1.5F;
		sample->m_boolValue = true;
		sample->m_stringValue = "hello";

		WHEN("ToJson is called")
		{
			const std::string out = cct::refl::Json::ToJson(*sample);

			THEN("Each member appears with its expected JSON form")
			{
				CHECK_THAT(out, ContainsSubstring("\"intValue\":42"));
				CHECK_THAT(out, ContainsSubstring("\"floatValue\":1.5"));
				CHECK_THAT(out, ContainsSubstring("\"boolValue\":true"));
				CHECK_THAT(out, ContainsSubstring("\"stringValue\":\"hello\""));
			}

			THEN("The output is a flat JSON object")
			{
				REQUIRE_FALSE(out.empty());
				CHECK(out.front() == '{');
				CHECK(out.back() == '}');
			}
		}
	}
}

SCENARIO("Json::ToJson - default values")
{
	GIVEN("A default-constructed JsonSample")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto sample = MakeSample();

		WHEN("ToJson is called")
		{
			const std::string out = cct::refl::Json::ToJson(*sample);

			THEN("Numeric defaults are emitted as 0 and bool as false")
			{
				CHECK_THAT(out, ContainsSubstring("\"intValue\":0"));
				CHECK_THAT(out, ContainsSubstring("\"floatValue\":0"));
				CHECK_THAT(out, ContainsSubstring("\"boolValue\":false"));
				CHECK_THAT(out, ContainsSubstring("\"stringValue\":\"\""));
			}
		}
	}
}

SCENARIO("Json::ToJson - escapes string special characters")
{
	GIVEN("A JsonSample whose string contains JSON special characters")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto sample = MakeSample();
		sample->m_stringValue = std::string("a\"b\\c\nd\te\x01"
											"f");

		WHEN("ToJson is called")
		{
			const std::string out = cct::refl::Json::ToJson(*sample);

			THEN("Each special character is escaped according to the JSON spec")
			{
				CHECK_THAT(out, ContainsSubstring("\"stringValue\":\"a\\\"b\\\\c\\nd\\te\\u0001f\""));
			}

			THEN("Round-trip restores the original string")
			{
				auto restored = MakeSample();
				REQUIRE(cct::refl::Json::FromJson(*restored, out));
				CHECK(restored->m_stringValue == sample->m_stringValue);
			}
		}
	}
}

SCENARIO("Json::FromJson - hydrates each native primitive type")
{
	GIVEN("A default JsonSample")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto sample = MakeSample();

		WHEN("FromJson parses a JSON object covering every member")
		{
			const std::string_view input =
				R"({"intValue":7,"floatValue":2.25,"boolValue":true,"stringValue":"world"})";

			REQUIRE(cct::refl::Json::FromJson(*sample, input));

			THEN("Each native member is populated")
			{
				CHECK(sample->m_intValue == 7);
				CHECK_THAT(static_cast<double>(sample->m_floatValue), WithinAbs(2.25, 1e-6));
				CHECK(sample->m_boolValue == true);
				CHECK(sample->m_stringValue == "world");
			}
		}
	}
}

SCENARIO("Json::FromJson - tolerates partial input and unknown keys")
{
	GIVEN("A JsonSample preloaded with sentinel values")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto sample = MakeSample();
		sample->m_intValue = 99;
		sample->m_floatValue = 9.5F;
		sample->m_boolValue = true;
		sample->m_stringValue = "untouched";

		WHEN("FromJson parses input that only sets intValue and includes an unknown key")
		{
			const std::string_view input = R"({"intValue":1,"unknownKey":"ignored"})";
			REQUIRE(cct::refl::Json::FromJson(*sample, input));

			THEN("Only the matched member is updated; unknowns are ignored without error")
			{
				CHECK(sample->m_intValue == 1);
				CHECK(sample->m_floatValue == 9.5F);
				CHECK(sample->m_boolValue == true);
				CHECK(sample->m_stringValue == "untouched");
			}
		}
	}
}

SCENARIO("Json::FromJson - reports failure on malformed input")
{
	GIVEN("A default JsonSample")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto sample = MakeSample();

		WHEN("FromJson is called with non-JSON text")
		{
			THEN("It returns false")
			{
				CHECK_FALSE(cct::refl::Json::FromJson(*sample, "not json"));
			}
		}

		WHEN("FromJson is called with a JSON value that is not an object")
		{
			THEN("It returns false")
			{
				CHECK_FALSE(cct::refl::Json::FromJson(*sample, "[1,2,3]"));
			}
		}
	}
}

SCENARIO("Json - round-trip preserves all primitive members")
{
	GIVEN("A populated JsonSample")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto original = MakeSample();
		original->m_intValue = -1234;
		original->m_floatValue = 3.140625F; // exactly representable in float
		original->m_boolValue = false;
		original->m_stringValue = "ok";

		WHEN("Serializing then deserializing into a fresh instance")
		{
			const std::string serialized = cct::refl::Json::ToJson(*original);
			auto roundTripped = MakeSample();
			REQUIRE(cct::refl::Json::FromJson(*roundTripped, serialized));

			THEN("All members match the original")
			{
				CHECK(roundTripped->m_intValue == original->m_intValue);
				CHECK_THAT(static_cast<double>(roundTripped->m_floatValue),
						   WithinAbs(static_cast<double>(original->m_floatValue), 1e-6));
				CHECK(roundTripped->m_boolValue == original->m_boolValue);
				CHECK(roundTripped->m_stringValue == original->m_stringValue);
			}
		}
	}
}
