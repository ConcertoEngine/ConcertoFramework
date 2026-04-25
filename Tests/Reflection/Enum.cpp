//
// Created by arthur on 12/31/2024.
//
#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/Enumeration/Enumeration.refl.hpp>
#include <Concerto/Reflection/Enumeration/EnumerationClass.hpp>
#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Enumeration metadata verification")
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

			THEN("We can get the SampleEnum enumeration class")
			{
				const auto* enumClass = cct::refl::GetClassByName("cct::sample::SampleEnum");
				REQUIRE(enumClass);
				CHECK(enumClass->GetName() == "SampleEnum"sv);

				const auto* enumClassPtr = dynamic_cast<const cct::refl::EnumerationClass*>(enumClass);
				REQUIRE(enumClassPtr);

				THEN("We can verify the enum values")
				{
					CHECK(enumClassPtr->GetEnumValueCount() == 2);

					const auto* fooValue = enumClassPtr->GetEnumValue("Foo"sv);
					REQUIRE(fooValue);
					CHECK(fooValue->GetName() == "Foo"sv);
					CHECK(fooValue->GetValue() == 0);
					CHECK(fooValue->GetIndex() == 0);

					const auto* barValue = enumClassPtr->GetEnumValue("Bar"sv);
					REQUIRE(barValue);
					CHECK(barValue->GetName() == "Bar"sv);
					CHECK(barValue->GetValue() == 1);
					CHECK(barValue->GetIndex() == 1);

					const auto* fooByValue = enumClassPtr->GetEnumValue(static_cast<cct::Int64>(0));
					REQUIRE(fooByValue);
					CHECK(fooByValue->GetName() == "Foo"sv);

					const auto* barByValue = enumClassPtr->GetEnumValue(static_cast<cct::Int64>(1));
					REQUIRE(barByValue);
					CHECK(barByValue->GetName() == "Bar"sv);

					const auto* fooByIndex = enumClassPtr->GetEnumValue(static_cast<std::size_t>(0));
					REQUIRE(fooByIndex);
					CHECK(fooByIndex->GetName() == "Foo"sv);

					const auto* barByIndex = enumClassPtr->GetEnumValue(static_cast<std::size_t>(1));
					REQUIRE(barByIndex);
					CHECK(barByIndex->GetName() == "Bar"sv);
				}

				THEN("We can create and use an enumeration instance")
				{
					auto enumInstance = enumClassPtr->CreateDefaultObject();
					REQUIRE(enumInstance);

					auto* enumPtr = dynamic_cast<cct::refl::Enumeration*>(enumInstance.get());
					REQUIRE(enumPtr);

					THEN("We can convert enum values to strings")
					{
						CHECK(enumPtr->ToString(0) == "Foo"sv);
						CHECK(enumPtr->ToString(1) == "Bar"sv);
						// Unknown value should return numeric string
						std::string unknownStr = enumPtr->ToString(999);
						CHECK((unknownStr == "999"sv));
					}

					THEN("We can convert enum names to values")
					{
						CHECK(enumPtr->FromString("Foo"sv) == 0);
						CHECK(enumPtr->FromString("Bar"sv) == 1);
						CHECK(enumPtr->FromString("Unknown"sv) == 0);
					}

					THEN("We can test flag operations")
					{
						CHECK(enumPtr->HasFlag(0, 0) == true); // 0 & 0 == 0, true
						CHECK(enumPtr->HasFlag(1, 1) == true); // 1 & 1 == 1, true
						CHECK(enumPtr->HasFlag(1, 0) == true); // 1 & 0 == 0, true (flag 0 is always present)

						cct::Int64 combined = enumPtr->SetFlag(0, 1);
						CHECK(combined == 1); // 0 | 1 = 1

						combined = enumPtr->SetFlag(0, 0);
						CHECK(combined == 0); // 0 | 0 = 0

						cct::Int64 toggled = enumPtr->ToggleFlag(0, 1);
						CHECK(toggled == 1); // 0 ^ 1 = 1

						toggled = enumPtr->ToggleFlag(1, 1);
						CHECK(toggled == 0); // 1 ^ 1 = 0
					}
				}
			}

			THEN("We can retrieve enum values from the enumeration class")
			{
				const auto* enumClass = dynamic_cast<const cct::refl::EnumerationClass*>(
					cct::refl::GetClassByName("cct::sample::SampleEnum"));
				REQUIRE(enumClass);

				auto enumValues = enumClass->GetEnumValues();
				CHECK(enumValues.size() == 2);

				if (enumValues.size() >= 2)
				{
					CHECK(enumValues[0]->GetName() == "Foo"sv);
					CHECK(enumValues[0]->GetValue() == 0);

					CHECK(enumValues[1]->GetName() == "Bar"sv);
					CHECK(enumValues[1]->GetValue() == 1);
				}
			}

			THEN("We can iterate over enum values with range-based for loops")
			{
				const auto* enumClass = dynamic_cast<const cct::refl::EnumerationClass*>(
					cct::refl::GetClassByName("cct::sample::SampleEnum"));
				REQUIRE(enumClass);

				std::size_t count = 0;
				for (auto [name, value] : enumClass->Enumerate())
				{
					count++;
					if (count == 1)
					{
						CHECK(name == "Foo"sv);
						CHECK(value == 0);
					}
					else if (count == 2)
					{
						CHECK(name == "Bar"sv);
						CHECK(value == 1);
					}
				}
				CHECK(count == 2);
			}
		}
	}
}
