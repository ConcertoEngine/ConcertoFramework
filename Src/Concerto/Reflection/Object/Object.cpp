//
// Created by arthur on 10/11/2024.
//

#include <vector>

#include <Concerto/Core/TypeInfo/TypeInfo.hpp>

#include "Concerto/Reflection/Boolean/Boolean.refl.hpp"
#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/Enumeration/Enumeration.refl.hpp"
#include "Concerto/Reflection/FieldVisitor/FieldVisitor.hpp"
#include "Concerto/Reflection/File/File.refl.hpp"
#include "Concerto/Reflection/Float32/Float32.refl.hpp"
#include "Concerto/Reflection/Float64/Float64.refl.hpp"
#include "Concerto/Reflection/Folder/Folder.refl.hpp"
#include "Concerto/Reflection/Int16/Int16.refl.hpp"
#include "Concerto/Reflection/Int32/Int32.refl.hpp"
#include "Concerto/Reflection/Int64/Int64.refl.hpp"
#include "Concerto/Reflection/Int8/Int8.refl.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"
#include "Concerto/Reflection/Object/Object.refl.hpp"
#include "Concerto/Reflection/Registry/Registry.hpp"
#include "Concerto/Reflection/String/String.refl.hpp"
#include "Concerto/Reflection/UInt16/UInt16.refl.hpp"
#include "Concerto/Reflection/UInt32/UInt32.refl.hpp"
#include "Concerto/Reflection/UInt64/UInt64.refl.hpp"
#include "Concerto/Reflection/UInt8/UInt8.refl.hpp"
#include "Concerto/Reflection/Vector/Vector.refl.hpp"

namespace cct::refl
{
	Object::Object() :
		m_dynamicClass(nullptr),
		m_registry(nullptr),
		m_uuid(cct::Uuid::Generate())
	{
	}

	Object::~Object()
	{
		// Registry tracking is identity-bound: only the object that was actually handed to
		// Registry::Track() (this exact address) may be present in its catalog.
		if (m_registry != nullptr)
			m_registry->Untrack(*this);
	}

	Object::Object(const Object& other)
	{
		m_dynamicClass = other.m_dynamicClass;
		m_registry = nullptr; // copy → new identity, not auto-tracked
		m_handle = {};
		m_uuid = cct::Uuid::Generate(); // copy → new identity
	}

	Object::Object(Object&& other) noexcept
	{
		m_dynamicClass = std::exchange(other.m_dynamicClass, nullptr);
		// `other` keeps its own registry entry (still keyed on &other) until it is destroyed;
		// `this` is a different address that was never tracked, so it starts untracked.
		m_registry = nullptr;
		m_handle = {};
		m_uuid = cct::Uuid::Generate(); // move → new address, new identity; `other` keeps its catalog key
	}

	Object& Object::operator=(const Object& other)
	{
		if (this == &other)
			return *this;

		m_dynamicClass = other.m_dynamicClass;
		// Registry tracking is tied to construction/destruction, not to value assignment.
		m_uuid = cct::Uuid::Generate();

		return *this;
	}

	Object& Object::operator=(Object&& other) noexcept
	{
		std::swap(m_dynamicClass, other.m_dynamicClass);
		// Deliberately not swapping m_registry nor m_uuid: both objects keep the catalog
		// entry that matches their own (unchanged) address, and its uuid key with it.

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
		if (m_uuid == uuid)
			return;
		const std::string previousKey = m_uuid.ToString();
		m_uuid = uuid;
		if (m_registry != nullptr)
		{
			m_registry->RekeyUuid(*this, previousKey);
		}
	}

	void Object::RegenerateUuid()
	{
		SetUuid(cct::Uuid::Generate());
	}

	void Object::Accept(FieldVisitor& visitor)
	{
		const Class* klass = GetDynamicClass();
		if (klass == nullptr)
			return;

		std::vector<const Class*> chain;
		for (const Class* c = klass; c != nullptr && c->GetName() != "Object"; c = c->GetBaseClass())
		{
			chain.push_back(c);
		}

		for (auto it = chain.rbegin(); it != chain.rend(); ++it)
		{
			const Class* c = *it;

			for (const auto& nmv : c->GetNativeMemberVariables())
			{
				if (!nmv)
					continue;
				const std::size_t idx = nmv->GetIndex();
				const cct::UInt64 typeId = nmv->GetTypeId();

				if (typeId == cct::TypeId<int>())
				{
					if (int* p = GetNativeMemberVariable<int>(idx))
						visitor.Visit(nmv->GetName(), *p);
				}
				else if (typeId == cct::TypeId<float>())
				{
					if (float* p = GetNativeMemberVariable<float>(idx))
						visitor.Visit(nmv->GetName(), *p);
				}
				else if (typeId == cct::TypeId<bool>())
				{
					if (bool* p = GetNativeMemberVariable<bool>(idx))
						visitor.Visit(nmv->GetName(), *p);
				}
				else if (typeId == cct::TypeId<std::string>())
				{
					if (std::string* p = GetNativeMemberVariable<std::string>(idx))
						visitor.Visit(nmv->GetName(), *p);
				}
			}

			for (const auto& mv : c->GetMemberVariables())
			{
				if (!mv)
					continue;
				Object* memberObj = c->GetMemberVariable(mv->GetIndex(), *this);
				if (memberObj == nullptr)
					continue;

				const std::string_view name = mv->GetName();

				if (memberObj->IsVector())
				{
					visitor.Visit(name, static_cast<Vector&>(*memberObj));
					continue;
				}
				if (memberObj->IsEnumeration())
				{
					visitor.Visit(name, static_cast<Enumeration&>(*memberObj));
					continue;
				}

				const Class* memberClass = memberObj->GetDynamicClass();
				if (memberClass == nullptr)
				{
					visitor.Visit(name, *memberObj);
					continue;
				}
				const std::string_view typeName = memberClass->GetName();

				if (typeName == "Boolean")
				{
					visitor.Visit(name, static_cast<Boolean&>(*memberObj));
				}
				else if (typeName == "Int8")
				{
					visitor.Visit(name, static_cast<Int8&>(*memberObj));
				}
				else if (typeName == "Int16")
				{
					visitor.Visit(name, static_cast<Int16&>(*memberObj));
				}
				else if (typeName == "Int32")
				{
					visitor.Visit(name, static_cast<Int32&>(*memberObj));
				}
				else if (typeName == "Int64")
				{
					visitor.Visit(name, static_cast<Int64&>(*memberObj));
				}
				else if (typeName == "UInt8")
				{
					visitor.Visit(name, static_cast<UInt8&>(*memberObj));
				}
				else if (typeName == "UInt16")
				{
					visitor.Visit(name, static_cast<UInt16&>(*memberObj));
				}
				else if (typeName == "UInt32")
				{
					visitor.Visit(name, static_cast<UInt32&>(*memberObj));
				}
				else if (typeName == "UInt64")
				{
					visitor.Visit(name, static_cast<UInt64&>(*memberObj));
				}
				else if (typeName == "Float32")
				{
					visitor.Visit(name, static_cast<Float32&>(*memberObj));
				}
				else if (typeName == "Float64")
				{
					visitor.Visit(name, static_cast<Float64&>(*memberObj));
				}
				else if (typeName == "String")
				{
					visitor.Visit(name, static_cast<String&>(*memberObj));
				}
				else if (typeName == "File")
				{
					visitor.Visit(name, static_cast<File&>(*memberObj));
				}
				else if (typeName == "Folder")
				{
					visitor.Visit(name, static_cast<Folder&>(*memberObj));
				}
				else
				{
					visitor.Visit(name, *memberObj);
				}
			}
		}
	}
} // namespace cct::refl
