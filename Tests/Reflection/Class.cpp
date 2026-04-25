//
// Created by arthur on 09/12/2024.
//
#define CATCH_CONFIG_RUNNER
#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Class metadata verification")
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

			THEN("We are getting the class Object")
			{
				const auto* klass = cct::refl::GetClassByName("cct::refl::Object");
				REQUIRE(klass);
				CHECK(klass == cct::refl::Object::GetClass());
				CHECK(klass->GetName() == "Object"sv);
				CHECK(klass->GetBaseClass() == nullptr);
				CHECK(klass->GetMemberVariableCount() == 0);
				CHECK(klass->GetMethodCount() == 0);
			}

			THEN("We are getting the class Int32")
			{
				const auto* klass = cct::refl::GetClassByName("cct::refl::Int32");
				REQUIRE(klass);
				CHECK(klass == cct::refl::Int32::GetClass());
				CHECK(klass->GetName() == "Int32"sv);
				CHECK(klass->GetBaseClass() == cct::refl::Object::GetClass());
				CHECK(klass->GetMemberVariableCount() == 0);
				CHECK(klass->GetMethodCount() == 0);
			}
		}
	}
}