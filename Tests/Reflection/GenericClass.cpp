//
// Created by arthur on 12/16/2025.
//
#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/GenericClass/GenericClass.hpp>
#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>

#include <catch2/catch_test_macros.hpp>
#include <SampleBar.refl.hpp>

SCENARIO("GenericClass identification")
{
	using namespace std::string_view_literals;
	GIVEN("Multiple class types in the system")
	{
		cct::refl::PackageLoader packageLoader;
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionPackage()));
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionTestsPackage()));
		packageLoader.LoadPackages();

		WHEN("Checking template vs generic vs regular classes")
		{
			THEN("Regular classes are neither template nor generic")
			{
				const auto* objectClass = cct::refl::GetClassByName("cct::refl::Object");
				REQUIRE(objectClass != nullptr);
				CHECK(objectClass->IsTemplateClass() == false);
				CHECK(objectClass->IsGenericClass() == false);

				const auto* sampleBarClass = cct::refl::GetClassByName("cct::sample::SampleBar");
				REQUIRE(sampleBarClass != nullptr);
				CHECK(sampleBarClass->IsTemplateClass() == false);
				CHECK(sampleBarClass->IsGenericClass() == false);
			}
		}
	}
}

SCENARIO("GenericPair multi-parameter generic class")
{
	using namespace std::string_view_literals;
	GIVEN("GenericPair class loaded")
	{
		cct::refl::PackageLoader packageLoader;
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionPackage()));
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionTestsPackage()));
		packageLoader.LoadPackages();

		WHEN("Looking up GenericPair class")
		{
			const auto* genericPairClass = cct::refl::GetClassByName("cct::sample::GenericPair");

			THEN("GenericPair is found and identified as generic")
			{
				REQUIRE(genericPairClass != nullptr);
				CHECK(genericPairClass->IsGenericClass() == true);
			}

			THEN("GenericPair has two type parameters")
			{
				REQUIRE(genericPairClass != nullptr);
				const auto* genericClass = dynamic_cast<const cct::refl::GenericClass*>(genericPairClass);
				REQUIRE(genericClass != nullptr);
				CHECK(genericClass->GetTypeParameterCount() == 2);
			}

			THEN("GenericPair type parameters are m_keyType and m_valueType")
			{
				REQUIRE(genericPairClass != nullptr);
				const auto* genericClass = dynamic_cast<const cct::refl::GenericClass*>(genericPairClass);
				REQUIRE(genericClass != nullptr);

				auto paramNames = genericClass->GetTypeParameterNames();
				REQUIRE(paramNames.size() == 2);
				CHECK(paramNames[0] == "m_keyType"sv);
				CHECK(paramNames[1] == "m_valueType"sv);
			}

			THEN("GenericPair can be instantiated with two type arguments")
			{
				REQUIRE(genericPairClass != nullptr);
				const auto* genericClass = dynamic_cast<const cct::refl::GenericClass*>(genericPairClass);
				REQUIRE(genericClass != nullptr);

				const auto* int32Class = cct::refl::GetClassByName("cct::refl::Int32");
				const auto* int64Class = cct::refl::GetClassByName("cct::refl::Int64");
				REQUIRE(int32Class != nullptr);
				REQUIRE(int64Class != nullptr);

				std::vector<const cct::refl::Class*> typeArgs = {int32Class, int64Class};
				auto obj = genericClass->CreateDefaultObject(
					std::span<const cct::refl::Class*>(typeArgs));
				REQUIRE(obj != nullptr);
				CHECK(obj->GetDynamicClass() == genericPairClass);

				auto genericPair = dynamic_cast<cct::sample::GenericPair*>(obj.get());
				REQUIRE(genericPair != nullptr);
				CHECK(genericPair->GetKeyType() == int32Class);
				CHECK(genericPair->GetValueType() == int64Class);
			}
		}
	}
}
