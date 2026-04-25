//
// Created by arthur on 31/12/2025
//

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Reflection/Enumeration/Enumeration.refl.hpp"
#include "Concerto/Reflection/Enumeration/EnumerationClass.hpp"
#include "Concerto/Reflection/Enumeration/EnumIterator.hpp"

namespace cct::refl
{
	Enumeration::Enumeration() :
		m_value(0)
	{
	}

	std::string Enumeration::ToString(cct::Int64 value) const
	{
		if (const auto* enumClass = dynamic_cast<const EnumerationClass*>(m_dynamicClass))
		{
			const EnumValue* enumValue = enumClass->GetEnumValue(value);
			if (enumValue)
				return std::string(enumValue->GetName());
		}

		const EnumValue* enumValue = GetEnumValue(value);
		if (enumValue)
			return std::string(enumValue->GetName());

		CCT_ASSERT_FALSE("Enumeration::ToString: enum value not found: {}", value);
		return std::to_string(value);
	}

	std::string Enumeration::ToString() const
	{
		return ToString(m_value);
	}

	cct::Int64 Enumeration::FromString(std::string_view name) const
	{
		if (const auto* enumClass = dynamic_cast<const EnumerationClass*>(m_dynamicClass))
		{
			const EnumValue* enumValue = enumClass->GetEnumValue(name);
			if (enumValue)
				return enumValue->GetValue();
		}

		const EnumValue* enumValue = GetEnumValue(name);
		if (enumValue)
			return enumValue->GetValue();

		CCT_ASSERT_FALSE("Enumeration::FromString: enum value not found: {}", name);
		return 0;
	}

	bool Enumeration::HasFlag(cct::Int64 value, cct::Int64 flag) const
	{
		return (value & flag) == flag;
	}

	cct::Int64 Enumeration::SetFlag(cct::Int64 value, cct::Int64 flag) const
	{
		return value | flag;
	}

	cct::Int64 Enumeration::ToggleFlag(cct::Int64 value, cct::Int64 flag) const
	{
		return value ^ flag;
	}

	EnumIterable Enumeration::Enumerate() const
	{
		return EnumIterable(m_enumValues);
	}

	void Enumeration::AddEnumValue(std::string name, cct::Int64 value)
	{
		std::size_t index = m_enumValues.size();
		m_enumValues.emplace_back(std::make_unique<EnumValue>(std::move(name), value, index));
	}

	void Enumeration::SetEnumValue(cct::Int64 value)
	{
		m_value = value;
	}
} // namespace cct::refl
