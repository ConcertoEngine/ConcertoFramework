//
// Created by arthur on 09/12/2024.
//
#define CATCH_CONFIG_RUNNER
#include <ConcertoReflectionPackage.gen.hpp>

#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Package initialization and destruction")
{
	using namespace std::string_view_literals;

	GIVEN("The core package is initialized")
	{
		auto pkg = CreateConcertoReflectionPackage();
		REQUIRE(pkg);

		pkg->LoadNamespaces();
		pkg->InitializeNamespaces();
		pkg->InitializeClasses();

		THEN("It must have the namespace 'cct'")
		{
			CHECK(pkg->GetNamespaceCount() == 1);
			auto* cctNamespace = pkg->GetNamespace("cct"sv);
			REQUIRE(cctNamespace);
			CHECK(cctNamespace->GetName() == "cct"sv);
		}

		AND_THEN("The GlobalNamespace must also have the namespace 'cct'")
		{
			auto* cctNamespaceG = cct::refl::GlobalNamespace::Get().GetNamespaceByName("cct");
			REQUIRE(cctNamespaceG);
			CHECK(cctNamespaceG->GetName() == "cct"sv);
		}

		AND_THEN("The namespace 'cct' must contain the namespace 'refl'")
		{
			auto* cctNamespace = pkg->GetNamespace("cct"sv);
			REQUIRE(cctNamespace);
			CHECK(cctNamespace->GetNamespaceCount() == 1);

			auto* reflNamespace = cctNamespace->GetNamespace("refl"sv);
			REQUIRE(reflNamespace);
			CHECK(reflNamespace->GetName() == "refl"sv);
		}

		AND_THEN("The namespace 'refl' must contain the class 'Object'")
		{
			auto* cctNamespace = pkg->GetNamespace("cct"sv);
			REQUIRE(cctNamespace);

			auto* reflNamespace = cctNamespace->GetNamespace("refl"sv);
			REQUIRE(reflNamespace);

			CHECK(reflNamespace->GetClassCount() >= 1);
			auto* objClass = reflNamespace->GetClass("Object"sv);
			REQUIRE(objClass);
			CHECK(objClass->GetName() == "Object"sv);
			CHECK(objClass->GetNamespaceName() == "refl"sv);
		}

		WHEN("The package is destroyed")
		{
			pkg = nullptr;

			THEN("GlobalNamespace should be empty")
			{
				CHECK(cct::refl::GlobalNamespace::Get().GetNamespaceByName("cct") == nullptr);
				CHECK(cct::refl::GlobalNamespace::Get().GetClassCount() == 0);
				CHECK(cct::refl::GlobalNamespace::Get().GetNamespaceCount() == 0);
				CHECK(cct::refl::Object::GetClass() == nullptr);
			}
		}
	}
}
