//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"

namespace cct::refl
{
	Object::Object() :
		m_dynamicClass(nullptr),
		m_registry(nullptr)
	{
	}

	Object::Object(const Object& other)
	{
		m_dynamicClass = other.m_dynamicClass;
		m_registry = other.m_registry;
	}

	Object::Object(Object&& other) noexcept
	{
		m_dynamicClass = std::exchange(other.m_dynamicClass, nullptr);
		m_registry = std::exchange(other.m_registry, nullptr);
	}

	Object& Object::operator=(const Object& other)
	{
		if (this == &other)
			return *this;

		m_dynamicClass = other.m_dynamicClass;
		m_registry = other.m_registry;

		return *this;
	}

	Object& Object::operator=(Object&& other) noexcept
	{
		std::swap(m_dynamicClass, other.m_dynamicClass);
		std::swap(m_registry, other.m_registry);

		return *this;
	}
} // namespace cct::refl
