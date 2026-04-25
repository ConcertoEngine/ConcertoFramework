//
// Created by arthur on 31/12/2025
//

#include "Concerto/Reflection/Enumeration/EnumerationClass.hpp"

#include "Concerto/Reflection/Enumeration/Enumeration.refl.hpp"
#include "Concerto/Reflection/Enumeration/EnumIterator.hpp"
#include "Concerto/Reflection/Namespace/Namespace.hpp"

namespace cct::refl
{
	EnumerationClass::EnumerationClass(Namespace* nameSpace, std::string name) :
		Class(nameSpace, std::move(name), nullptr)
	{
	}

	std::span<const std::unique_ptr<EnumValue>> EnumerationClass::GetEnumValues() const
	{
		return m_enumValues;
	}

	std::size_t EnumerationClass::GetEnumValueCount() const
	{
		return m_enumValues.size();
	}

	const EnumValue* EnumerationClass::GetEnumValue(std::size_t index) const
	{
		if (index >= m_enumValues.size())
			return nullptr;
		return m_enumValues[index].get();
	}

	const EnumValue* EnumerationClass::GetEnumValue(std::string_view name) const
	{
		for (const auto& enumValue : m_enumValues)
		{
			if (enumValue && enumValue->GetName() == name)
				return enumValue.get();
		}

		CCT_ASSERT_FALSE("Could not find enum value with name: {}", name);
		return nullptr;
	}

	const EnumValue* EnumerationClass::GetEnumValue(cct::Int64 value) const
	{
		for (const auto& enumValue : m_enumValues)
		{
			if (enumValue && enumValue->GetValue() == value)
				return enumValue.get();
		}

		CCT_ASSERT_FALSE("Could not find enum value with value: {}", value);
		return nullptr;
	}

	EnumIterable EnumerationClass::Enumerate() const
	{
		return EnumIterable(m_enumValues);
	}

	std::unique_ptr<Object> EnumerationClass::CreateDefaultObject() const
	{
		auto enumeration = std::make_unique<Enumeration>();
		enumeration->SetDynamicClass(this);
		return enumeration;
	}

	void EnumerationClass::AddEnumValue(std::string name, cct::Int64 value)
	{
		std::size_t index = m_enumValues.size();
		m_enumValues.emplace_back(std::make_unique<EnumValue>(std::move(name), value, index));
	}

	void EnumerationClass::AddDynamicEnumValue(Enumeration& enumeration, std::string name, cct::Int64 value) const
	{
		enumeration.AddEnumValue(std::move(name), value);
	}

	void EnumerationClass::Initialize()
	{
	}
} // namespace cct::refl
