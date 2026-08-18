//
// Created by arthur on 17/08/2026.
//
#define CATCH_CONFIG_RUNNER
#include <ConcertoReflectionPackage.gen.hpp>
#include <ConcertoReflectionTestsPackage.gen.hpp>

#include <Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp>
#include <Concerto/Reflection/PackageLoader/PackageLoader.hpp>
#include <Concerto/Reflection/Registry/Registry.hpp>

#include "SampleBar.refl.hpp"
#include <catch2/catch_test_macros.hpp>

static void LoadPackages(cct::refl::PackageLoader& loader)
{
	REQUIRE(loader.AddPackage(CreateConcertoReflectionPackage()));
	REQUIRE(loader.AddPackage(CreateConcertoReflectionTestsPackage()));
	loader.LoadPackages();
}

SCENARIO("Registry - CreateDefaultObject() alone leaves an object untracked")
{
	GIVEN("The reflection packages are loaded, with no registry in scope")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		WHEN("An object is created via Class::CreateDefaultObject() directly")
		{
			auto instance = sampleBarClass->CreateDefaultObject();
			REQUIRE(instance != nullptr);

			THEN("It reports no registry — there is no implicit global to fall back to")
			{
				CHECK_FALSE(instance->HasRegistry());
				CHECK(instance->GetRegistry() == nullptr);
			}
		}
	}
}

SCENARIO("Registry - Allocate()/Create<T>() track the object into that registry")
{
	GIVEN("A Registry instance")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry registry;
		const std::size_t before = registry.GetObjectCount(sampleBarClass);

		WHEN("An object is created via Registry::Allocate()")
		{
			auto instance = registry.Allocate(sampleBarClass);
			REQUIRE(instance != nullptr);

			THEN("It reports this registry as its owner")
			{
				CHECK(instance->HasRegistry());
				CHECK(instance->GetRegistry() == &registry);
			}

			THEN("The registry catalog grows by one for that class")
			{
				CHECK(registry.GetObjectCount(sampleBarClass) == before + 1);

				bool found = false;
				for (cct::refl::Object* obj : registry.GetObjects(sampleBarClass))
					if (obj == instance.get())
						found = true;
				CHECK(found);
			}
		}

		WHEN("An object is created via the typed Registry::Create<T>() helper")
		{
			auto instance = registry.Create<cct::sample::SampleBar>();
			REQUIRE(instance != nullptr);
			CHECK(instance->HasRegistry());
			CHECK(instance->GetRegistry() == &registry);
		}

		THEN("Destroying the tracked object removes it from the catalog again")
		{
			{
				auto instance = registry.Allocate(sampleBarClass);
				REQUIRE(instance != nullptr);
				CHECK(registry.GetObjectCount(sampleBarClass) == before + 1);
			}
			CHECK(registry.GetObjectCount(sampleBarClass) == before);
		}
	}
}

SCENARIO("Registry - copies are untracked, moves keep the original entry")
{
	GIVEN("A tracked SampleBar instance")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry registry;
		auto instance = registry.Allocate(sampleBarClass);
		REQUIRE(instance != nullptr);
		REQUIRE(instance->HasRegistry());

		WHEN("It is copy-constructed")
		{
			cct::sample::SampleBar copy(static_cast<const cct::sample::SampleBar&>(*instance));

			THEN("The copy is a new identity and is not tracked")
			{
				CHECK_FALSE(copy.HasRegistry());
				CHECK(copy.GetUuid() != instance->GetUuid());
			}

			THEN("The original is still tracked")
			{
				CHECK(instance->HasRegistry());
			}
		}
	}
}

SCENARIO("Registry - Track() re-homes an object out of its previous registry")
{
	GIVEN("An object tracked in one registry")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry first;
		cct::refl::Registry second;

		auto instance = first.Allocate(sampleBarClass);
		REQUIRE(instance != nullptr);
		REQUIRE(instance->GetRegistry() == &first);
		REQUIRE(first.GetObjectCount(sampleBarClass) == 1);

		WHEN("It is tracked into a second registry")
		{
			second.Track(*instance);

			THEN("It now belongs to the second registry only")
			{
				CHECK(instance->GetRegistry() == &second);
				CHECK(second.GetObjectCount(sampleBarClass) == 1);
				CHECK(first.GetObjectCount(sampleBarClass) == 0);
			}
		}
	}
}

SCENARIO("Registry - Track() issues a handle that resolves back to the object")
{
	GIVEN("A Registry instance")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry registry;

		WHEN("An object is allocated through it")
		{
			auto instance = registry.Allocate(sampleBarClass);
			REQUIRE(instance != nullptr);

			THEN("It carries a non-null handle that resolves to itself")
			{
				const cct::refl::Handle handle = instance->GetHandle();
				CHECK_FALSE(handle.IsNull());
				CHECK(registry.IsValid(handle));
				CHECK(registry.Resolve(handle) == instance.get());
			}
		}

		WHEN("A default-constructed handle is resolved")
		{
			THEN("It is null and resolves to nothing")
			{
				const cct::refl::Handle handle;
				CHECK(handle.IsNull());
				CHECK_FALSE(registry.IsValid(handle));
				CHECK(registry.Resolve(handle) == nullptr);
			}
		}
	}
}

SCENARIO("Registry - a handle stops resolving once its object is gone")
{
	GIVEN("A tracked object and its handle")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry registry;
		auto instance = registry.Allocate(sampleBarClass);
		REQUIRE(instance != nullptr);
		const cct::refl::Handle handle = instance->GetHandle();
		REQUIRE(registry.IsValid(handle));

		WHEN("The object is explicitly untracked")
		{
			registry.Untrack(*instance);

			THEN("The handle no longer resolves")
			{
				CHECK_FALSE(registry.IsValid(handle));
				CHECK(registry.Resolve(handle) == nullptr);
			}

			THEN("The object itself reports a null handle")
			{
				CHECK(instance->GetHandle().IsNull());
			}
		}

		WHEN("The object is destroyed")
		{
			instance.reset();

			THEN("The stale handle is detected instead of dangling")
			{
				CHECK_FALSE(registry.IsValid(handle));
				CHECK(registry.Resolve(handle) == nullptr);
			}
		}
	}
}

SCENARIO("Registry - a recycled slot never resolves an older handle")
{
	GIVEN("A registry whose only object has been destroyed")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry registry;

		auto first = registry.Allocate(sampleBarClass);
		REQUIRE(first != nullptr);
		const cct::refl::Handle stale = first->GetHandle();
		first.reset();

		WHEN("A new object reuses the freed slot")
		{
			auto second = registry.Allocate(sampleBarClass);
			REQUIRE(second != nullptr);
			const cct::refl::Handle fresh = second->GetHandle();

			THEN("The fresh handle resolves to the new object")
			{
				CHECK(registry.Resolve(fresh) == second.get());
			}

			THEN("The stale handle still resolves to nothing")
			{
				CHECK_FALSE(registry.IsValid(stale));
				CHECK(registry.Resolve(stale) == nullptr);
			}

			THEN("The two handles are not equal")
			{
				CHECK_FALSE(stale == fresh);
			}
		}
	}
}

SCENARIO("Registry - re-homing issues a fresh handle and voids the previous one")
{
	GIVEN("An object tracked in one registry")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry first;
		cct::refl::Registry second;

		auto instance = first.Allocate(sampleBarClass);
		REQUIRE(instance != nullptr);
		const cct::refl::Handle firstHandle = instance->GetHandle();
		REQUIRE(first.IsValid(firstHandle));

		WHEN("It is tracked into a second registry")
		{
			second.Track(*instance);
			const cct::refl::Handle secondHandle = instance->GetHandle();

			THEN("The new handle resolves in the second registry")
			{
				CHECK(second.Resolve(secondHandle) == instance.get());
			}

			THEN("The old handle no longer resolves in the first registry")
			{
				CHECK_FALSE(first.IsValid(firstHandle));
				CHECK(first.Resolve(firstHandle) == nullptr);
			}
		}
	}
}

SCENARIO("Registry - Adopt() takes ownership and keeps the object alive")
{
	GIVEN("A Registry instance")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry registry;

		WHEN("An object is adopted")
		{
			auto instance = sampleBarClass->CreateDefaultObject();
			REQUIRE(instance != nullptr);
			cct::refl::Object* raw = instance.get();

			const cct::refl::Handle handle = registry.Adopt(std::move(instance));

			THEN("The caller's unique_ptr is emptied but the object is alive and tracked")
			{
				CHECK(instance == nullptr);
				CHECK_FALSE(handle.IsNull());
				CHECK(registry.Resolve(handle) == raw);
				CHECK(registry.Owns(handle));
				CHECK(registry.GetObjectCount(sampleBarClass) == 1);
			}
		}

		WHEN("A null unique_ptr is adopted")
		{
			const cct::refl::Handle handle = registry.Adopt(nullptr);

			THEN("A null handle comes back and nothing is tracked")
			{
				CHECK(handle.IsNull());
				CHECK(registry.GetObjectCount() == 0);
			}
		}

		WHEN("A merely tracked object is queried for ownership")
		{
			auto tracked = registry.Allocate(sampleBarClass);
			REQUIRE(tracked != nullptr);

			THEN("The registry tracks it without owning it")
			{
				CHECK(registry.IsValid(tracked->GetHandle()));
				CHECK_FALSE(registry.Owns(tracked->GetHandle()));
			}
		}
	}
}

SCENARIO("Registry - Release() hands ownership back to the caller")
{
	GIVEN("An adopted object")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry registry;
		const cct::refl::Handle handle = registry.Adopt(sampleBarClass->CreateDefaultObject());
		REQUIRE(registry.Owns(handle));

		WHEN("It is released")
		{
			std::unique_ptr<cct::refl::Object> owned = registry.Release(handle);

			THEN("The caller gets the object back, untracked")
			{
				REQUIRE(owned != nullptr);
				CHECK_FALSE(owned->HasRegistry());
				CHECK(owned->GetHandle().IsNull());
				CHECK(registry.GetObjectCount(sampleBarClass) == 0);
			}

			THEN("The handle is void")
			{
				CHECK_FALSE(registry.IsValid(handle));
				CHECK_FALSE(registry.Owns(handle));
			}
		}

		WHEN("Release is called twice")
		{
			std::unique_ptr<cct::refl::Object> first = registry.Release(handle);
			std::unique_ptr<cct::refl::Object> second = registry.Release(handle);

			THEN("Only the first call yields the object")
			{
				CHECK(first != nullptr);
				CHECK(second == nullptr);
			}
		}
	}
}

SCENARIO("Registry - Destroy() voids the handle immediately but defers the free")
{
	GIVEN("An adopted object")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry registry;
		const cct::refl::Handle handle = registry.Adopt(sampleBarClass->CreateDefaultObject());
		REQUIRE(registry.IsValid(handle));

		WHEN("Destroy is called")
		{
			registry.Destroy(handle);

			THEN("The handle stops resolving straight away")
			{
				CHECK_FALSE(registry.IsValid(handle));
				CHECK(registry.Resolve(handle) == nullptr);
			}

			THEN("The object is no longer in the catalog")
			{
				CHECK(registry.GetObjectCount(sampleBarClass) == 0);
			}

			THEN("But it has not been freed yet")
			{
				CHECK(registry.GetPendingDestructionCount() == 1);
			}

			AND_WHEN("Garbage is collected")
			{
				registry.CollectGarbage();

				THEN("The pending queue is drained")
				{
					CHECK(registry.GetPendingDestructionCount() == 0);
				}
			}
		}

		WHEN("Destroy is called on a non-owned object")
		{
			auto tracked = registry.Allocate(sampleBarClass);
			REQUIRE(tracked != nullptr);
			const cct::refl::Handle trackedHandle = tracked->GetHandle();

			registry.Destroy(trackedHandle);

			THEN("Nothing is queued and the object stays valid")
			{
				CHECK(registry.GetPendingDestructionCount() == 0);
				CHECK(registry.IsValid(trackedHandle));
			}
		}
	}
}

SCENARIO("Registry - destroying the registry frees what it owns")
{
	GIVEN("A registry owning an object whose destruction is observable")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		WHEN("The registry goes out of scope with an owned and a pending object")
		{
			bool survived = true;
			{
				cct::refl::Registry registry;
				registry.Adopt(sampleBarClass->CreateDefaultObject());
				const cct::refl::Handle doomed = registry.Adopt(sampleBarClass->CreateDefaultObject());
				registry.Destroy(doomed);
				REQUIRE(registry.GetPendingDestructionCount() == 1);
			}

			THEN("Teardown completes without re-entering the destroyed catalog")
			{
				CHECK(survived);
			}
		}
	}
}

SCENARIO("Registry - resolves tracked objects by uuid")
{
	GIVEN("A registry holding several objects")
	{
		cct::refl::PackageLoader loader;
		LoadPackages(loader);

		const cct::refl::Class* sampleBarClass = cct::sample::SampleBar::GetClass();
		REQUIRE(sampleBarClass != nullptr);

		cct::refl::Registry registry;
		auto first = registry.Allocate(sampleBarClass);
		auto second = registry.Allocate(sampleBarClass);
		auto third = registry.Allocate(sampleBarClass);
		REQUIRE(first != nullptr);
		REQUIRE(second != nullptr);
		REQUIRE(third != nullptr);

		WHEN("Each uuid is looked up")
		{
			THEN("It resolves to the object that owns it")
			{
				CHECK(registry.FindByUuid(first->GetUuid().ToString()) == first.get());
				CHECK(registry.FindByUuid(second->GetUuid().ToString()) == second.get());
				CHECK(registry.FindByUuid(third->GetUuid().ToString()) == third.get());
			}
		}

		WHEN("An unknown uuid is looked up")
		{
			THEN("Nothing is returned")
			{
				CHECK(registry.FindByUuid("not-a-real-uuid") == nullptr);
			}
		}

		WHEN("An object in the middle is destroyed")
		{
			const std::string survivingUuid = third->GetUuid().ToString();
			const std::string goneUuid = second->GetUuid().ToString();
			second.reset();

			THEN("Its uuid stops resolving")
			{
				CHECK(registry.FindByUuid(goneUuid) == nullptr);
			}

			THEN("The remaining objects still resolve, and the catalog stays consistent")
			{
				CHECK(registry.FindByUuid(survivingUuid) == third.get());
				CHECK(registry.FindByUuid(first->GetUuid().ToString()) == first.get());
				CHECK(registry.GetObjectCount(sampleBarClass) == 2);
			}
		}

		WHEN("Every object is destroyed in creation order")
		{
			const std::string firstUuid = first->GetUuid().ToString();
			first.reset();
			second.reset();
			third.reset();

			THEN("The catalog empties without leaving stale entries")
			{
				CHECK(registry.GetObjectCount(sampleBarClass) == 0);
				CHECK(registry.FindByUuid(firstUuid) == nullptr);
			}
		}
	}
}
