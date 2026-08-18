//
// Created by arthur on 18/08/2026.
//
#define CATCH_CONFIG_RUNNER
#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/Binary/Binary.hpp>
#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/Json/Json.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>

#include "SampleBar.refl.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

namespace
{
	void LoadPackages(cct::refl::PackageLoader& loader)
	{
		REQUIRE(loader.AddPackage(CreateConcertoReflectionPackage()));
		REQUIRE(loader.AddPackage(CreateConcertoReflectionTestsPackage()));
		loader.LoadPackages();
	}

	std::unique_ptr<cct::sample::JsonSample> MakeSample()
	{
		auto obj = cct::sample::JsonSample::GetClass()->CreateDefaultObject<cct::sample::JsonSample>();
		REQUIRE(obj != nullptr);
		return obj;
	}
} // namespace

SCENARIO("Binary - round-trips every native primitive")
{
	GIVEN("A JsonSample populated with mixed primitive values")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto source = MakeSample();
		source->m_intValue = 42;
		source->m_floatValue = 1.5F;
		source->m_boolValue = true;
		source->m_stringValue = "hello";

		WHEN("It is serialized to binary and read back into a fresh object")
		{
			const std::vector<std::byte> blob = cct::refl::Binary::ToBinary(*source);
			auto restored = MakeSample();

			REQUIRE(cct::refl::Binary::FromBinary(*restored, blob));

			THEN("Every member survives the round-trip")
			{
				CHECK(restored->m_intValue == 42);
				CHECK_THAT(static_cast<double>(restored->m_floatValue), WithinAbs(1.5, 1e-6));
				CHECK(restored->m_boolValue == true);
				CHECK(restored->m_stringValue == "hello");
			}
		}
	}
}

SCENARIO("Binary - is more compact than the JSON encoding")
{
	GIVEN("The same object encoded both ways")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto sample = MakeSample();
		sample->m_intValue = 123456;
		sample->m_floatValue = 3.14159F;
		sample->m_boolValue = true;
		sample->m_stringValue = "a reasonably typical string value";

		WHEN("Both encodings are produced")
		{
			const std::string json = cct::refl::Json::ToJson(*sample);
			const std::vector<std::byte> blob = cct::refl::Binary::ToBinary(*sample);

			THEN("The binary payload is strictly smaller")
			{
				CHECK(blob.size() < json.size());
			}
		}
	}
}

SCENARIO("Binary - tolerates missing fields without clobbering existing values")
{
	GIVEN("A payload produced from a partially populated object")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto source = MakeSample();
		source->m_intValue = 7;

		const std::vector<std::byte> blob = cct::refl::Binary::ToBinary(*source);

		WHEN("It is read into an object carrying sentinel values")
		{
			auto target = MakeSample();
			target->m_intValue = 99;
			target->m_stringValue = "sentinel";

			REQUIRE(cct::refl::Binary::FromBinary(*target, blob));

			THEN("Present fields are applied")
			{
				CHECK(target->m_intValue == 7);
			}

			THEN("Fields the source left at their default are applied as that default")
			{
				CHECK(target->m_stringValue.empty());
			}
		}
	}
}

SCENARIO("Binary - rejects a truncated payload instead of reading past the end")
{
	GIVEN("A valid payload")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		auto source = MakeSample();
		source->m_stringValue = "some content that makes the blob longer";
		const std::vector<std::byte> blob = cct::refl::Binary::ToBinary(*source);
		REQUIRE(blob.size() > 8);

		WHEN("Only the first few bytes are handed to the parser")
		{
			const std::span<const std::byte> truncated{blob.data(), 5};
			auto target = MakeSample();
			const bool ok = cct::refl::Binary::FromBinary(*target, truncated);

			THEN("The parser reports failure rather than crashing")
			{
				CHECK_FALSE(ok);
			}
		}

		WHEN("An empty payload is handed to the parser")
		{
			auto target = MakeSample();
			const bool ok = cct::refl::Binary::FromBinary(*target, {});

			THEN("It fails cleanly")
			{
				CHECK_FALSE(ok);
			}
		}
	}
}
