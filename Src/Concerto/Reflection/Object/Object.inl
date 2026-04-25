//
// Created by arthur on 10/11/2024.
//

#pragma once

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	inline bool Object::IsInstanceOf(const Class& klass)
	{
		return GetClass()->InheritsFrom(klass);
	}

	inline const cct::refl::Object* Object::GetMemberVariable(std::size_t index) const
	{
		return GetDynamicClass()->GetMemberVariable(index, *this);
	}

	inline cct::refl::Object* Object::GetMemberVariable(std::size_t index)
	{
		return GetDynamicClass()->GetMemberVariable(index, *this);
	}

	inline const cct::refl::Object* Object::GetMemberVariable(std::string_view name) const
	{
		return GetDynamicClass()->GetMemberVariable(name, *this);
	}

	inline cct::refl::Object* Object::GetMemberVariable(std::string_view name)
	{
		return GetDynamicClass()->GetMemberVariable(name, *this);
	}

	inline const cct::refl::Class* Object::GetDynamicClass() const
	{
		return m_dynamicClass;
	}

	inline void Object::SetDynamicClass(const Class* klass)
	{
		CCT_ASSERT(m_dynamicClass == nullptr, "Dynamic class variable already set");
		m_dynamicClass = klass;
	}

	inline bool Object::HasRegistry() const
	{
		return m_registry != nullptr;
	}

	inline Registry* Object::GetRegistry()
	{
		return m_registry;
	}

	inline const Registry* Object::GetRegistry() const
	{
		return m_registry;
	}

	inline void Object::InitializeMemberVariables()
	{
		for (const auto& m : GetDynamicClass()->GetMemberVariables())
		{
			if (m == nullptr)
				continue; // should never happen

			auto* variable = GetMemberVariable(m->GetIndex());
			if (variable == nullptr)
				continue; // should never happen

			variable->SetDynamicClass(m->GetType());
			variable->InitializeMemberVariables();
		}
	}

	template<typename T>
	T* Object::GetMemberVariable(std::size_t index)
	{
		Object* object = GetClass()->GetMemberVariable(index, *this);
#ifdef CCT_DEBUG
		if (dynamic_cast<T*>(object) == nullptr) [[unlikely]]
		{
			CCT_ASSERT_FALSE("'{}' is not convertible to '{}'", this->GetClass()->GetName(), T::GetClass()->GetName());
			return nullptr;
		}
#endif
		return static_cast<T*>(object);
	}

	template<typename T>
	const T* Object::GetMemberVariable(std::size_t index) const
	{
		Object* object = GetClass()->GetMemberVariable(index, *this);
#ifdef CCT_DEBUG
		if (dynamic_cast<T*>(object) == nullptr) [[unlikely]]
		{
			CCT_ASSERT_FALSE("'{}' is not convertible to '{}'", this->GetClass()->GetName(), T::GetClass()->GetName());
			return nullptr;
		}
#endif
		return static_cast<T*>(object);
	}

	template<typename T>
	T* Object::GetMemberVariable(std::string_view name)
	{
		const MemberVariable* memberVariable = GetClass()->GetMemberVariable(name);
		if (memberVariable == nullptr)
			return nullptr;
		return GetMemberVariable<T>(memberVariable->GetIndex());
	}

	template<typename T>
	const T* Object::GetMemberVariable(std::string_view name) const
	{
		const MemberVariable* memberVariable = GetDynamicClass()->GetMemberVariable(name);
		if (memberVariable == nullptr)
			return nullptr;
		return GetMemberVariable<T>(memberVariable->GetIndex());
	}

	template<typename T>
	T* Object::GetNativeMemberVariable(std::size_t index)
	{
		void* variable = GetDynamicClass()->GetNativeMemberVariable(index, *this);
#ifdef CCT_DEBUG
		const NativeMemberVariable* memberVariable = GetDynamicClass()->GetNativeMemberVariable(index);
		if (memberVariable->GetTypeId() != cct::TypeId<T>())
		{
			CCT_ASSERT_FALSE("Invalid type");
			return nullptr;
		}
#endif
		return static_cast<T*>(variable);
	}

	template<typename T>
	const T* Object::GetNativeMemberVariable(std::size_t index) const
	{
		void* variable = GetDynamicClass()->GetNativeMemberVariable(index, *this);
#ifdef CCT_DEBUG
		const NativeMemberVariable* memberVariable = GetDynamicClass()->GetNativeMemberVariable(index);
		if (memberVariable->GetTypeId() != cct::TypeId<T>())
		{
			CCT_ASSERT_FALSE("Invalid type");
			return nullptr;
		}
#endif
		return static_cast<T*>(variable);
	}

	template<typename T>
	T* Object::GetNativeMemberVariable(std::string_view name)
	{
		const NativeMemberVariable* memberVariable = GetDynamicClass()->GetNativeMemberVariable(name);
		if (memberVariable == nullptr)
			return nullptr;
		return GetNativeMemberVariable<T>(memberVariable->GetIndex());
	}

	template<typename T>
	const T* Object::GetNativeMemberVariable(std::string_view name) const
	{
		const NativeMemberVariable* memberVariable = GetDynamicClass()->GetNativeMemberVariable(name);
		if (memberVariable == nullptr)
			return nullptr;
		return GetNativeMemberVariable<T>(memberVariable->GetIndex());
	}
} // namespace cct::refl
