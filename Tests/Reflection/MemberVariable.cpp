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

SCENARIO("MemberVariable")
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

			THEN("We are getting the member variable of 'SampleBar'")
			{
				const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
				REQUIRE(sampleBarClass);

				const size_t memberVariableCount = sampleBarClass->GetMemberVariableCount();
				REQUIRE(memberVariableCount > 0);
				CHECK(memberVariableCount == 1);

				auto sampleBarInstance = sampleBarClass->CreateDefaultObject();
				REQUIRE(sampleBarInstance);

				const cct::refl::Object* sampleBar1 = cct::sample::SampleBar::GetClass()->GetMemberVariable("bar", *sampleBarInstance);
				REQUIRE(sampleBar1);
				CHECK(sampleBar1->GetDynamicClass() == cct::refl::Int32::GetClass());

				const cct::refl::Object* sampleBar2 = sampleBarInstance->GetDynamicClass()->GetMemberVariable(0, *sampleBarInstance);
				CHECK(sampleBar1 == sampleBar2);

				sampleBar2 = sampleBarInstance->GetMemberVariable("bar");
				CHECK(sampleBar1 == sampleBar2);

				sampleBar2 = sampleBarInstance->GetMemberVariable(0);
				CHECK(sampleBar1 == sampleBar2);

				const cct::refl::MemberVariable* sampleBarMemberVariable1 = cct::sample::SampleBar::GetClass()->GetMemberVariable("bar");
				REQUIRE(sampleBarMemberVariable1);
				CHECK(sampleBarMemberVariable1->GetName() == "bar"sv);
				CHECK(sampleBarMemberVariable1->GetIndex() == 0);
				CHECK(sampleBarMemberVariable1->GetType() == cct::refl::Int32::GetClass());

				const cct::refl::MemberVariable* sampleBarMemberVariable2 = cct::sample::SampleBar::GetClass()->GetMemberVariable(0);
				CHECK(sampleBarMemberVariable1 == sampleBarMemberVariable2);
			}

			THEN("We are getting the native member variable of 'SampleBar'")
			{
				const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
				REQUIRE(sampleBarClass);
				CHECK(sampleBarClass->GetNativeMemberVariableCount() == 1);

				auto sampleBarInstance = sampleBarClass->CreateDefaultObject();
				REQUIRE(sampleBarInstance);

				// Global native-member indices are offset by the base class's own native
				// members (e.g. cct::refl::Object::m_uuid) — SampleBar::m_nativeType is
				// declared first locally, but not necessarily at global index 0.
				const cct::refl::Class* baseClass = sampleBarClass->GetBaseClass();
				REQUIRE(baseClass);
				const std::size_t baseNativeCount = baseClass->GetTotalNativeMemberCount();

				const cct::refl::NativeMemberVariable* nativeMemberVariable = sampleBarClass->GetNativeMemberVariable(baseNativeCount);
				REQUIRE(nativeMemberVariable);
				CHECK(nativeMemberVariable->GetName() == "nativeType"sv);
				CHECK(nativeMemberVariable->GetIndex() == baseNativeCount);
				CHECK(nativeMemberVariable->GetTypeId() == cct::TypeId<cct::Int32>());

				const cct::refl::NativeMemberVariable* nativeMemberVariable1 = sampleBarClass->GetNativeMemberVariable("nativeType"sv);
				REQUIRE(nativeMemberVariable1);
				CHECK(nativeMemberVariable1->GetName() == "nativeType"sv);
				CHECK(nativeMemberVariable1->GetIndex() == baseNativeCount);
				CHECK(nativeMemberVariable1->GetTypeId() == cct::TypeId<cct::Int32>());

				const cct::Int32* memberVariable1 = sampleBarInstance->GetNativeMemberVariable<cct::Int32>(baseNativeCount);
				REQUIRE(memberVariable1);
				CHECK(*memberVariable1 == 42);

				const cct::Int32* memberVariable2 = sampleBarInstance->GetNativeMemberVariable<cct::Int32>("nativeType");
				CHECK(memberVariable2);
				CHECK(memberVariable2 == memberVariable1);
			}
		}
	}
}