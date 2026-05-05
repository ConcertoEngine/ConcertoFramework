//
// Created by arthur on 03/05/2026.
//

#pragma once

#include <concepts>
#include <typeinfo>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	template<typename To, typename From>
		requires(std::is_base_of_v<Object, To> && std::is_base_of_v<Object, From>)
	[[nodiscard]] To* Cast(From* from)
	{
		if (from == nullptr)
			return nullptr;
		const Class* fromClass = from->GetDynamicClass();
		const Class* toClass   = To::GetClass();
		if (fromClass == nullptr || toClass == nullptr)
			return nullptr;
		if (fromClass == toClass || fromClass->InheritsFrom(*toClass))
		{
#ifdef CCT_DEBUG
			if (dynamic_cast<To*>(from) == nullptr)
			{
				CCT_ASSERT(false, "cct::refl::Cast: reflection approved {} -> {} but dynamic_cast disagrees, reflection hierarchy is incorrect. typeid: {}", fromClass->GetName(), toClass->GetName(), typeid(*from).name());
				return nullptr;
			}
#endif
			return static_cast<To*>(from);
		}
		return nullptr;
	}

	template<typename To, typename From>
		requires(std::is_base_of_v<Object, To> && std::is_base_of_v<Object, From>)
	[[nodiscard]] const To* Cast(const From* from)
	{
		if (from == nullptr)
			return nullptr;
		const Class* fromClass = from->GetDynamicClass();
		const Class* toClass   = To::GetClass();
		if (fromClass == nullptr || toClass == nullptr)
			return nullptr;
		if (fromClass == toClass || fromClass->InheritsFrom(*toClass))
		{
#ifdef CCT_DEBUG
			if (dynamic_cast<const To*>(from) == nullptr)
			{
				CCT_ASSERT(false, "cct::refl::Cast: reflection approved {} -> {} but dynamic_cast disagrees, reflection hierarchy is incorrect. typeid: {}", fromClass->GetName(), toClass->GetName(), typeid(*from).name());
				return nullptr;
			}
#endif
			return static_cast<const To*>(from);
		}
		return nullptr;
	}
} // namespace cct::refl
