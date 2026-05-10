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
		m_registry(nullptr),
		m_uuid(cct::Uuid::Generate())
	{
	}

	Object::Object(const Object& other)
	{
		m_dynamicClass = other.m_dynamicClass;
		m_registry = other.m_registry;
		m_uuid = cct::Uuid::Generate(); // copy → new identity
	}

	Object::Object(Object&& other) noexcept
	{
		m_dynamicClass = std::exchange(other.m_dynamicClass, nullptr);
		m_registry = std::exchange(other.m_registry, nullptr);
		m_uuid = std::exchange(other.m_uuid, {});
	}

	Object& Object::operator=(const Object& other)
	{
		if (this == &other)
			return *this;

		m_dynamicClass = other.m_dynamicClass;
		m_registry = other.m_registry;
		m_uuid = cct::Uuid::Generate();

		return *this;
	}

	Object& Object::operator=(Object&& other) noexcept
	{
		std::swap(m_dynamicClass, other.m_dynamicClass);
		std::swap(m_registry, other.m_registry);
		std::swap(m_uuid, other.m_uuid);

		return *this;
	}

	std::string Object::ToString() const
	{
		return "";
	}

	const cct::Uuid& Object::GetUuid() const
	{
		return m_uuid;
	}

	void Object::SetUuid(const cct::Uuid& uuid)
	{
		m_uuid = uuid;
	}

	void Object::RegenerateUuid()
	{
		m_uuid = cct::Uuid::Generate();
	}
} // namespace cct::refl
