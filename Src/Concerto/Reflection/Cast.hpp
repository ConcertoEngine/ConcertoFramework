//
// Created by arthur on 03/05/2026.
//

#pragma once

#include <concepts>
#include <type_traits>
#include <typeinfo>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	// Deduce the return pointer type from To:
	//   Cast<Foo>(from)        = Foo*         (To is a non-pointer class)
	//   Cast<Foo*>(from)       = Foo*         (To is already a pointer)
	//   Cast<const Foo*>(from) = const Foo*   (To is already a const pointer)
	template<typename To>
	using CastPtrType = std::conditional_t<std::is_pointer_v<To>, To, To*>;

	template<typename To>
	using CastConstPtrType = std::conditional_t<std::is_pointer_v<To>, To, const To*>;

	// Strip pointer + cv to get the plain class type for GetClass() lookup.
	template<typename To>
	using CastBaseType = std::remove_pointer_t<std::remove_cv_t<To>>;

	template<typename To, typename From>
		requires(std::is_base_of_v<Object, std::remove_pointer_t<std::remove_cvref_t<From>>> &&
				 std::is_base_of_v<Object, CastBaseType<To>>)
	[[nodiscard]] CastPtrType<To> Cast(From* from)
	{
		using ToPtr = CastPtrType<To>;
		if (from == nullptr)
			return nullptr;
		const Class* fromClass = from->GetDynamicClass();
		const Class* toClass = CastBaseType<To>::GetClass();
		if (fromClass == nullptr || toClass == nullptr)
			return nullptr;
		if (fromClass == toClass || fromClass->InheritsFrom(*toClass))
		{
#ifdef CCT_DEBUG
			if (dynamic_cast<ToPtr>(from) == nullptr)
			{
				CCT_ASSERT(false, "cct::refl::Cast: reflection approved {} -> {} but dynamic_cast disagrees, reflection hierarchy is incorrect. typeid: {}", fromClass->GetName(), toClass->GetName(), typeid(*from).name());
				return nullptr;
			}
#endif
			return static_cast<ToPtr>(from);
		}
		return nullptr;
	}

	template<typename To, typename From>
		requires(std::is_base_of_v<Object, std::remove_pointer_t<std::remove_cvref_t<From>>> &&
				 std::is_base_of_v<Object, CastBaseType<To>>)
	[[nodiscard]] CastConstPtrType<To> Cast(const From* from)
	{
		using ToPtr = CastConstPtrType<To>;
		if (from == nullptr)
			return nullptr;
		const Class* fromClass = from->GetDynamicClass();
		const Class* toClass = CastBaseType<To>::GetClass();
		if (fromClass == nullptr || toClass == nullptr)
			return nullptr;
		if (fromClass == toClass || fromClass->InheritsFrom(*toClass))
		{
#ifdef CCT_DEBUG
			if (dynamic_cast<ToPtr>(from) == nullptr)
			{
				CCT_ASSERT(false, "cct::refl::Cast: reflection approved {} -> {} but dynamic_cast disagrees, reflection hierarchy is incorrect. typeid: {}", fromClass->GetName(), toClass->GetName(), typeid(*from).name());
				return nullptr;
			}
#endif
			return static_cast<ToPtr>(from);
		}
		return nullptr;
	}
} // namespace cct::refl
