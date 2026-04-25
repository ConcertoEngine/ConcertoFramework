//
// Created by arthur on 17/12/2024.
//
#define CATCH_CONFIG_RUNNER
#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Namespace")
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

			THEN("We are getting the cct namespace")
			{
				const auto* cctNs = cct::refl::GlobalNamespace::Get().GetNamespaceByName("cct");
				REQUIRE(cctNs);
				CHECK(cctNs->GetName() == "cct"sv);
				CHECK(cctNs->HasClass("Object"sv) == false);
				CHECK(cctNs->GetClass("refl::Object"sv) == cct::refl::Object::GetClass());
				CHECK(cctNs->GetClass("refl::Foo"sv) == nullptr);
				CHECK(cctNs->GetClass("refl"sv) == nullptr);
				CHECK(cctNs->GetClass(""sv) == nullptr);
				const auto* fooNs = cctNs->GetNamespace("foo"sv);
				CHECK(fooNs == nullptr);
			}

			THEN("We are getting the cct::refl namespace")
			{
				const auto* reflNs = cct::refl::GlobalNamespace::Get().GetNamespaceByName("cct::refl");
				REQUIRE(reflNs);
				CHECK(reflNs->GetName() == "refl"sv);
				CHECK(reflNs->HasClass("Object"sv));
				CHECK(reflNs->GetNamespaceCount() == 0);
			}

			THEN("We are getting a non existing namespace")
			{
				auto* nonExistingNs = cct::refl::GlobalNamespace::Get().GetNamespaceByName("foo");
				CHECK(nonExistingNs == nullptr);
				nonExistingNs = cct::refl::GlobalNamespace::Get().GetNamespaceByName("cct::foo");
				CHECK(nonExistingNs == nullptr);
			}
		}
	}
}