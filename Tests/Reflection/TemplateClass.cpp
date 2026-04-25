//
// Created by arthur on 10/12/2024.
//
#define CATCH_CONFIG_RUNNER
#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>
#include <Concerto/Reflection/TemplateClass/TemplateClass.hpp>

#include <catch2/catch_test_macros.hpp>

SCENARIO("TemplateClass API with loaded packages")
{
	using namespace std::string_view_literals;
	GIVEN("Loaded reflection packages with template support")
	{
		cct::refl::PackageLoader packageLoader;
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionPackage()));
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionTestsPackage()));
		packageLoader.LoadPackages();

		WHEN("Checking class IsTemplateClass property")
		{
			THEN("Non-template classes return false")
			{
				const auto* sampleBarClass = cct::refl::GetClassByName("cct::sample::SampleBar");
				REQUIRE(sampleBarClass != nullptr);
				CHECK(sampleBarClass->IsTemplateClass() == false);
			}

			THEN("Template specializations are identified as templates")
			{
				const auto* templatedPairClass = cct::refl::GetClassByName("cct::sample::TemplatedPair<int, float>");
				if (templatedPairClass != nullptr)
				{
					bool isTemplate = templatedPairClass->IsTemplateClass();
					CHECK(isTemplate == true);
				}
			}
		}

		WHEN("Accessing template specializations")
		{
			THEN("Specializations are retrievable")
			{
				const auto* templatedPairClass = cct::refl::GetClassByName("cct::sample::TemplatedPair<int, float>");
				if (templatedPairClass != nullptr && templatedPairClass->IsTemplateClass())
				{
					const auto* templateClass = dynamic_cast<const cct::refl::TemplateClass*>(templatedPairClass);
					REQUIRE(templateClass != nullptr);

					CHECK(templatedPairClass->GetName() == "TemplatedPair<int, float>");
				}
			}
		}
	}
}

SCENARIO("TemplateClass identification")
{
	using namespace std::string_view_literals;
	GIVEN("Multiple class types in the system")
	{
		cct::refl::PackageLoader packageLoader;
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionPackage()));
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionTestsPackage()));
		packageLoader.LoadPackages();

		WHEN("Checking template vs non-template classes")
		{
			THEN("Built-in classes are not templates")
			{
				const auto* objectClass = cct::refl::GetClassByName("cct::refl::Object");
				REQUIRE(objectClass != nullptr);
				CHECK(objectClass->IsTemplateClass() == false);

				const auto* int32Class = cct::refl::GetClassByName("cct::refl::Int32");
				REQUIRE(int32Class != nullptr);
				CHECK(int32Class->IsTemplateClass() == false);
			}

			THEN("User classes are not templates")
			{
				const auto* sampleBarClass = cct::refl::GetClassByName("cct::sample::SampleBar");
				REQUIRE(sampleBarClass != nullptr);
				CHECK(sampleBarClass->IsTemplateClass() == false);
			}

			THEN("Template specializations are templates")
			{
				const auto* templatedPairClass = cct::refl::GetClassByName("cct::sample::TemplatedPair<int, float>");
				if (templatedPairClass != nullptr)
				{
					CHECK(templatedPairClass->IsTemplateClass() == true);
				}
			}
		}
	}
}

SCENARIO("TemplateClass specialization retrieval by string")
{
	using namespace std::string_view_literals;
	GIVEN("Template class with specializations loaded")
	{
		cct::refl::PackageLoader packageLoader;
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionPackage()));
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionTestsPackage()));
		packageLoader.LoadPackages();

		WHEN("Getting specializations by template arguments")
		{
			THEN("Can retrieve TemplatedPair<int, float> specialization")
			{
				const auto* templatedPairClass = cct::refl::GetClassByName("cct::sample::TemplatedPair<int, float>");
				if (templatedPairClass != nullptr && templatedPairClass->IsTemplateClass())
				{
					const auto* templateClass = dynamic_cast<const cct::refl::TemplateClass*>(templatedPairClass);
					REQUIRE(templateClass);
					CHECK(templatedPairClass->GetName() == "TemplatedPair<int, float>");
				}
			}

			THEN("Can retrieve TemplatedPair<std::string, std::string> specialization")
			{
				const auto* templatedPairClass = cct::refl::GetClassByName("cct::sample::TemplatedPair<std::string, std::string>");
				if (templatedPairClass != nullptr && templatedPairClass->IsTemplateClass())
				{
					const auto* templateClass = dynamic_cast<const cct::refl::TemplateClass*>(templatedPairClass);
					REQUIRE(templateClass);
					CHECK(templatedPairClass->GetName() == "TemplatedPair<std::string, std::string>");
				}
			}
		}

		WHEN("Accessing specialized class methods")
		{
			THEN("Specialization classes have initialized structure")
			{
				const auto* templatedPairClass = cct::refl::GetClassByName("cct::sample::TemplatedPair<int, float>");
				if (templatedPairClass != nullptr && templatedPairClass->IsTemplateClass())
				{
					const auto* templateClass = dynamic_cast<const cct::refl::TemplateClass*>(templatedPairClass);
					REQUIRE(templateClass);
					CHECK(templatedPairClass->GetMethodCount() > 0);
					CHECK(templatedPairClass->GetMemberVariableCount() > 0);
				}
			}
		}
	}
}

SCENARIO("TemplateClass retrieval by GetClassByName")
{
	using namespace std::string_view_literals;
	GIVEN("Loaded reflection packages with template specializations")
	{
		cct::refl::PackageLoader packageLoader;
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionPackage()));
		REQUIRE(packageLoader.AddPackage(CreateConcertoReflectionTestsPackage()));
		packageLoader.LoadPackages();

		WHEN("Getting template specializations by name")
		{
			THEN("TemplatedPair specializations may or may not be in reflection")
			{
				const auto* notExistsSpec = cct::refl::GetClassByName("cct::sample::TemplatedPair<NotExists, float>");
				REQUIRE(notExistsSpec == nullptr);
			}

			THEN("Non-existent template specialization returns nullptr")
			{
				const auto* invalid = cct::refl::GetClassByName("cct::sample::TemplatedPair<float, float>");
				CHECK(invalid == nullptr);
			}
		}
	}
}
