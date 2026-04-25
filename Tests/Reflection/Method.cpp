//
// Created by arthur on 04/07/2025.
//
#define CATCH_CONFIG_RUNNER
#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>

#include "SampleBar.refl.hpp"
#include <catch2/catch_test_macros.hpp>

SCENARIO("Method")
{
	using namespace std::string_view_literals;
	GIVEN("The core package")
	{
		WHEN("The package is initialized")
		{
			cct::refl::PackageLoader packageLoader;
			REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionPackage()));
			REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionTestsPackage()));
			packageLoader.LoadPackages();

			CHECK(cct::refl::GlobalNamespace::Get().GetClassCount() == 15);
			CHECK(cct::refl::GlobalNamespace::Get().GetNamespaceCount() == 1);

			THEN("We are invoking a method named 'Bar'")
			{
				const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
				REQUIRE(sampleBarClass);

				const cct::refl::Method* barMethod = sampleBarClass->GetMethod("Bar");
				REQUIRE(barMethod);

				auto sampleBar = cct::sample::SampleBar::GetClass()->CreateDefaultObject<cct::sample::SampleBar>();
				REQUIRE(sampleBar);
				cct::refl::Int32 a;
				auto result = barMethod->Invoke<cct::refl::Int32>(*sampleBar, a, a, a);
				REQUIRE(result.IsOk());
				CHECK(result.GetValue() == 42);
			}

			THEN("We are invoking a method with a custom delegate")
			{
				const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
				REQUIRE(sampleBarClass);

				const cct::refl::Method* customDelegate = sampleBarClass->GetMethod("CustomDelegate");
				REQUIRE(customDelegate);

				auto sampleBar = cct::sample::SampleBar::GetClass()->CreateDefaultObject<cct::sample::SampleBar>();
				REQUIRE(sampleBar);
				cct::refl::Int32 a(0);
				auto result = customDelegate->Invoke<cct::refl::Int32>(*sampleBar);
				REQUIRE(result.IsOk());
				CHECK(result.GetValue() == a);
			}
		}
	}
}