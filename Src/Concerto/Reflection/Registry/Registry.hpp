//
// Created by arthur on 06/10/2025.
//

#ifndef CONCERTO_REFLECTION_REGISTRY_HPP
#define CONCERTO_REFLECTION_REGISTRY_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Registry/Handle.hpp"

namespace cct::refl
{
	class Object;
	class Class;

	/// A catalog of live cct::refl::Object instances, and — for objects handed to Adopt() —
	/// their owner.
	///
	/// Track() is non-owning: the object stays owned by whoever already held it, and removes
	/// itself from the catalog in ~Object(). Adopt() transfers ownership to the registry;
	/// the object then lives until Release(), Destroy() + CollectGarbage(), or ~Registry().
	/// There is no implicit/global registry — an object is only ever tracked when a caller
	/// explicitly routes its creation through one.
	///
	/// LIFETIME CONTRACT: a Registry must outlive every object it owns and every container
	/// holding handles into it. cct::refl::Vector adopts its elements into the registry it
	/// binds to on first insertion (see Vector::ElementRegistry), so a Registry passed to
	/// Json::FromJson / Binary::FromBinary must outlive the deserialized graph. A stack
	/// Registry used only for the duration of a deserialize call is a use-after-free.
	class CCT_REFLECTION_API Registry
	{
	public:
		explicit Registry(std::size_t defaultObjectCount = 0xFFF);
		~Registry();

		Registry(Registry&& other) noexcept;
		Registry& operator=(Registry&& other) noexcept;
		Registry(const Registry&) = delete;
		Registry& operator=(const Registry&) = delete;

		/// Creates an object of the given class and tracks it into this registry.
		/// Equivalent to klass->CreateDefaultObject(), except the result is guaranteed
		/// to be tracked here.
		std::unique_ptr<Object> Allocate(const Class* klass);

		/// Typed convenience over Allocate(T::GetClass()) for call sites that know the
		/// concrete type at compile time (e.g. an app's own domain objects).
		template<typename T>
			requires(std::is_base_of_v<Object, T> && std::is_polymorphic_v<T>)
		[[nodiscard]] std::unique_ptr<T> Create();

		/// Registers an already-constructed object into the catalog, re-homing it out of
		/// whichever registry (if any) currently tracks it. Safe to call again (no-op if
		/// the object is already tracked here).
		void Track(Object& object);

		/// Removes an object from the catalog. Called automatically from ~Object().
		void Untrack(Object& object);

		Handle Adopt(std::unique_ptr<Object> object);
		std::unique_ptr<Object> Release(Handle handle);
		void Destroy(Handle handle);
		void CollectGarbage();

		[[nodiscard]] bool Owns(Handle handle) const;
		[[nodiscard]] std::size_t GetPendingDestructionCount() const;

		[[nodiscard]] Object* Resolve(Handle handle) const;
		[[nodiscard]] Object* FindByUuid(std::string_view uuid) const;
		void RekeyUuid(Object& object, const std::string& previousKey);
		[[nodiscard]] bool IsValid(Handle handle) const;

		[[nodiscard]] std::size_t GetObjectCount() const;
		[[nodiscard]] std::size_t GetObjectCount(const Class* klass) const;
		[[nodiscard]] std::span<Object* const> GetObjects(const Class* klass) const;
		void ForEachObject(const std::function<void(Object&)>& visitor) const;

	private:
		struct Slot
		{
			Object* m_object = nullptr;
			std::unique_ptr<Object> m_owned;
			std::uint32_t m_generation = 0;
			std::uint32_t m_bucketIndex = 0;
		};

		Handle TrackInternal(Object& object, std::unique_ptr<Object> owned);
		Handle AcquireSlot(Object& object, std::unique_ptr<Object> owned);
		void ReleaseSlot(Handle handle);
		void IndexObject(const Class* klass, Object& object, Handle handle);
		void UnindexObject(Object& object);
		void DetachAll();
		void Rehome();

		std::size_t m_defaultObjectCount;
		std::unordered_map<const Class*, std::vector<Object*>> m_objects;
		std::vector<Slot> m_slots;
		std::vector<std::uint32_t> m_freeSlots;
		std::vector<std::unique_ptr<Object>> m_pendingDestruction;
		std::unordered_map<std::string, Object*> m_byUuid;
	};
} // namespace cct::refl

#include "Concerto/Reflection/Registry/Registry.inl"

#endif // CONCERTO_REFLECTION_REGISTRY_HPP