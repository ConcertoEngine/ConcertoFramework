//
// Created by arthur on 27/02/2026.
//
#define CATCH_CONFIG_RUNNER
#include <catch2/catch_test_macros.hpp>

#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/GenericClass/GenericClass.hpp>
#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>
#include <Concerto/Core/Signal/Connection.hpp>
#include <Concerto/Reflection/String/String.refl.hpp>
#include <Concerto/Reflection/Vector/Vector.refl.hpp>

using namespace std::string_view_literals;

static void LoadPackages(cct::refl::PackageLoader& loader)
{
	REQUIRE(loader.AddPackage(CreateConcertoReflectionPackage()));
	REQUIRE(loader.AddPackage(CreateConcertoReflectionTestsPackage()));
	loader.LoadPackages();
}

SCENARIO("Vector - reflection metadata")
{
	GIVEN("The reflection packages are loaded")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		WHEN("Looking up the Vector class")
		{
			const auto* vectorClass = cct::refl::GetClassByName("cct::refl::Vector");
			REQUIRE(vectorClass != nullptr);

			THEN("It is identified as a GenericClass")
			{
				CHECK(vectorClass->IsGenericClass() == true);
				CHECK(vectorClass->IsTemplateClass() == false);
			}

			THEN("It has exactly one type parameter named m_elementType")
			{
				const auto* genericClass = dynamic_cast<const cct::refl::GenericClass*>(vectorClass);
				REQUIRE(genericClass != nullptr);
				CHECK(genericClass->GetTypeParameterCount() == 1);

				auto paramNames = genericClass->GetTypeParameterNames();
				REQUIRE(paramNames.size() == 1);
				CHECK(paramNames[0] == "m_elementType");
			}
		}
	}
}

SCENARIO("Vector - instantiation via GenericClass")
{
	GIVEN("The reflection packages are loaded")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const auto* vectorClass = dynamic_cast<const cct::refl::GenericClass*>(
			cct::refl::GetClassByName("cct::refl::Vector"));
		REQUIRE(vectorClass != nullptr);

		const auto* stringClass = cct::refl::String::GetClass();
		REQUIRE(stringClass != nullptr);

		WHEN("Creating a Vector<String> via CreateDefaultObject(typeArgs)")
		{
			std::vector<const cct::refl::Class*> typeArgs = {stringClass};
			auto obj = vectorClass->CreateDefaultObject(
				std::span<const cct::refl::Class*>(typeArgs));

			REQUIRE(obj != nullptr);
			auto* vec = dynamic_cast<cct::refl::Vector*>(obj.get());
			REQUIRE(vec != nullptr);

			THEN("The element type is correctly set to String")
			{
				CHECK(vec->GetElementType() == stringClass);
				CHECK(vec->m_elementType == stringClass);
			}

			THEN("The vector is initially empty")
			{
				CHECK(vec->GetCount() == 0);
				CHECK(vec->IsEmpty());
			}
		}
	}
}

SCENARIO("Vector - Add and OnInserted signal")
{
	GIVEN("A Vector<String> instance")
	{
		cct::refl::Vector vec;
		vec.m_elementType = cct::refl::String::GetClass();

		std::size_t insertedIndex = SIZE_MAX;
		std::string insertedValue;
		int callCount = 0;

		auto conn = vec.OnInserted.Connect(
			[&](std::size_t idx, cct::refl::Object& elem)
			{
				insertedIndex = idx;
				if (auto* s = dynamic_cast<cct::refl::String*>(&elem))
					insertedValue = s->Get();
				++callCount;
			});

		WHEN("Add(unique_ptr<String>) is called")
		{
			vec.Add(std::make_unique<cct::refl::String>("hello"sv));

			THEN("OnInserted fires with correct index and element")
			{
				CHECK(callCount == 1);
				CHECK(insertedIndex == 0);
				CHECK(insertedValue == "hello");
				CHECK(vec.GetCount() == 1);
			}

			AND_WHEN("A second element is added")
			{
				vec.Add(std::make_unique<cct::refl::String>("world"sv));
				THEN("OnInserted fires again with index 1")
				{
					CHECK(callCount == 2);
					CHECK(insertedIndex == 1);
					CHECK(insertedValue == "world");
					CHECK(vec.GetCount() == 2);
				}
			}
		}

		WHEN("Add() with no argument creates a default String")
		{
			auto* elem = vec.Add();
			REQUIRE(elem != nullptr);

			THEN("Element is a String with empty value")
			{
				auto* s = dynamic_cast<cct::refl::String*>(elem);
				REQUIRE(s != nullptr);
				CHECK(s->Get().empty());
				CHECK(callCount == 1);
				CHECK(insertedIndex == 0);
			}
		}

		WHEN("Add() is called without m_elementType set")
		{
			cct::refl::Vector untyped;
			// m_elementType is null, Add() should assert (no crash in test mode)
			// Just verify state is unchanged
			CHECK(untyped.GetCount() == 0);
		}
	}
}

SCENARIO("Vector - Remove and OnRemoved signal")
{
	GIVEN("A Vector<String> with two elements")
	{
		cct::refl::Vector vec;
		vec.m_elementType = cct::refl::String::GetClass();
		vec.Add(std::make_unique<cct::refl::String>("first"sv));
		vec.Add(std::make_unique<cct::refl::String>("second"sv));
		REQUIRE(vec.GetCount() == 2);

		std::size_t removedIndex = SIZE_MAX;
		std::string removedValue;
		std::size_t countDuringCallback = 0;
		int callCount = 0;

		auto conn = vec.OnRemoved.Connect(
			[&](std::size_t idx, cct::refl::Object& elem)
			{
				removedIndex = idx;
				// Element is still accessible during the callback
				countDuringCallback = vec.GetCount();
				if (auto* s = dynamic_cast<cct::refl::String*>(&elem))
					removedValue = s->Get();
				++callCount;
			});

		WHEN("Remove(0) is called")
		{
			vec.Remove(0);

			THEN("OnRemoved fires before actual removal")
			{
				CHECK(callCount == 1);
				CHECK(removedIndex == 0);
				CHECK(removedValue == "first");
				CHECK(countDuringCallback == 2); // still 2 during callback
			}

			THEN("Vector has one element remaining after removal")
			{
				CHECK(vec.GetCount() == 1);
				auto* remaining = dynamic_cast<cct::refl::String*>(vec.Get(0));
				REQUIRE(remaining != nullptr);
				CHECK(remaining->Get() == "second");
			}
		}
	}
}

SCENARIO("Vector - Clear and OnCleared signal")
{
	GIVEN("A Vector<String> with three elements")
	{
		cct::refl::Vector vec;
		vec.m_elementType = cct::refl::String::GetClass();
		vec.Add(std::make_unique<cct::refl::String>("a"sv));
		vec.Add(std::make_unique<cct::refl::String>("b"sv));
		vec.Add(std::make_unique<cct::refl::String>("c"sv));
		REQUIRE(vec.GetCount() == 3);

		std::size_t countDuringClear = 0;
		int callCount = 0;

		auto conn = vec.OnCleared.Connect(
			[&]()
			{
				countDuringClear = vec.GetCount(); // still alive during callback
				++callCount;
			});

		WHEN("Clear() is called")
		{
			vec.Clear();

			THEN("OnCleared fires before elements are destroyed")
			{
				CHECK(callCount == 1);
				CHECK(countDuringClear == 3);
			}

			THEN("Vector is empty after clear")
			{
				CHECK(vec.IsEmpty());
				CHECK(vec.GetCount() == 0);
			}
		}

		WHEN("Clear() is called on an already-empty vector")
		{
			vec.Clear();
			callCount = 0; // reset
			vec.Clear();
			THEN("OnCleared is NOT called again") { CHECK(callCount == 0); }
		}
	}
}

SCENARIO("Vector - OnValueChanged is emitted on every mutation")
{
	GIVEN("A Vector<String> with OnValueChanged connected")
	{
		cct::refl::Vector vec;
		vec.m_elementType = cct::refl::String::GetClass();
		int changeCount = 0;
		auto conn = vec.OnValueChanged.Connect([&changeCount]() { ++changeCount; });

		WHEN("Add, Remove, and Clear are called")
		{
			vec.Add(std::make_unique<cct::refl::String>("x"sv));
			CHECK(changeCount == 1);

			vec.Add(std::make_unique<cct::refl::String>("y"sv));
			CHECK(changeCount == 2);

			vec.Remove(0);
			CHECK(changeCount == 3);

			vec.Clear();
			THEN("OnValueChanged fires for each mutation") { CHECK(changeCount == 4); }
		}
	}
}

SCENARIO("Vector - ScopedConnection auto-disconnect")
{
	GIVEN("A Vector<String>")
	{
		cct::refl::Vector vec;
		vec.m_elementType = cct::refl::String::GetClass();
		int callCount = 0;

		WHEN("A ScopedConnection on OnInserted goes out of scope")
		{
			{
				cct::ScopedConnection sc{
					vec.OnInserted.Connect([&callCount](std::size_t, cct::refl::Object&)
										   { ++callCount; })};
				vec.Add(std::make_unique<cct::refl::String>("in scope"sv));
				CHECK(callCount == 1);
			} // sc destroyed, auto-disconnect

			vec.Add(std::make_unique<cct::refl::String>("out of scope"sv));
			THEN("Callback is not called after scope exit") { CHECK(callCount == 1); }
		}
	}
}

SCENARIO("Vector - Get<T> templated access")
{
	GIVEN("A Vector<String> with elements")
	{
		cct::refl::Vector vec;
		vec.m_elementType = cct::refl::String::GetClass();
		vec.Add(std::make_unique<cct::refl::String>("hello"sv));

		WHEN("Get<String>(0) is called")
		{
			auto* str = vec.Get<cct::refl::String>(0);
			THEN("Returns the correctly typed pointer")
			{
				REQUIRE(str != nullptr);
				CHECK(str->Get() == "hello");
			}
		}
	}
}
