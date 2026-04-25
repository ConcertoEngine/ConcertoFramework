//
// Created by arthur on 27/02/2026.
//

#ifndef CONCERTO_REFLECTION_VECTOR_HPP
#define CONCERTO_REFLECTION_VECTOR_HPP

#include <cstddef>
#include <memory>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	/// A reflectable, observable collection of Object-derived elements.
	///
	/// Vector is a GenericClass: it is parameterized at runtime via its
	/// `m_elementType` type parameter. Create instances through the reflection system:
	///
	///   const auto* vecGenericClass =
	///       dynamic_cast<const cct::refl::GenericClass*>(
	///           cct::refl::GetClassByName("cct::refl::Vector"));
	///   auto vec = vecGenericClass->CreateDefaultObject({String::GetClass()});
	///
	/// Or use it directly as a standalone instance without reflection:
	///
	///   cct::refl::Vector strings;
	///   strings.m_elementType = cct::refl::String::GetClass();
	///   auto conn = strings.OnInserted.Connect([](std::size_t i, cct::refl::Object& elem) { ... });
	///   strings.Add(std::make_unique<cct::refl::String>("hello"));
	class CCT_REFL_CLASS() CCT_GENERIC_CLASS() CCT_REFLECTION_API Vector : public Object
	{
	public:
		Vector();
		~Vector() override = default;

		Vector(const Vector&) = delete;
		Vector(Vector&&) = default;
		Vector& operator=(const Vector&) = delete;
		Vector& operator=(Vector&&) = default;

		// ── Type parameter ────────────────────────────────────────────────
		/// The element type of this vector. Set by the code-generated
		/// CreateDefaultObject(typeArgs). May also be set directly.
		CCT_REFL_MEMBER()
		CCT_GENERIC_TYPE()
		const Class* m_elementType;

		// ── Signals ───────────────────────────────────────────────────────
		/// Emitted AFTER an element is inserted. Args: (index, element).
		Signal<std::size_t, Object&> OnInserted;

		/// Emitted BEFORE an element is removed. Args: (index, element).
		/// The element is still accessible during the callback.
		Signal<std::size_t, Object&> OnRemoved;

		/// Emitted BEFORE all elements are cleared.
		Signal<> OnCleared;

		// ── Mutation ──────────────────────────────────────────────────────
		/// Takes ownership of the element and appends it.
		/// Validates that the element's dynamic class matches m_elementType (if set).
		/// Emits OnInserted then OnValueChanged.
		void Add(std::unique_ptr<Object> element);

		/// Creates a default element of m_elementType, appends it, and returns a
		/// non-owning pointer to it.
		/// Requires m_elementType to be non-null.
		Object* Add();

		/// Removes the element at the given index.
		/// Emits OnRemoved (before removal) then OnValueChanged.
		void Remove(std::size_t index);

		/// Removes all elements.
		/// Emits OnCleared (before removal) then OnValueChanged.
		void Clear();

		// ── Read access ───────────────────────────────────────────────────
		[[nodiscard]] Object* Get(std::size_t index);
		[[nodiscard]] const Object* Get(std::size_t index) const;

		template<typename T>
		[[nodiscard]] T* Get(std::size_t index);
		template<typename T>
		[[nodiscard]] const T* Get(std::size_t index) const;

		[[nodiscard]] std::size_t GetCount() const;
		[[nodiscard]] bool IsEmpty() const;
		[[nodiscard]] const Class* GetElementType() const;

		// ── Iteration ─────────────────────────────────────────────────────
		auto begin() noexcept;
		auto end() noexcept;
		auto begin() const noexcept;
		auto end() const noexcept;

		CCT_OBJECT(Vector);

	private:
		std::vector<std::unique_ptr<Object>> m_elements; // NOT reflected
	};
} // namespace cct::refl

#include "Concerto/Reflection/Vector/Vector.inl"

#endif // CONCERTO_REFLECTION_VECTOR_HPP
