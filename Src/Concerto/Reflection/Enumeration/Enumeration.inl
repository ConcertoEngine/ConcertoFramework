//
// Created by arthur on 31/12/2025
//

#pragma once

#include "Concerto/Core/Assert.hpp"

namespace cct::refl
{
	inline std::span<const std::unique_ptr<EnumValue>> Enumeration::GetEnumValues() const
	{
		return m_enumValues;
	}

	inline std::size_t Enumeration::GetEnumValueCount() const
	{
		return m_enumValues.size();
	}

	inline const EnumValue* Enumeration::GetEnumValue(std::size_t index) const
	{
		if (index >= m_enumValues.size())
		{
			CCT_ASSERT_FALSE("Index out of range: {}", index);
			return nullptr;
		}
		return m_enumValues[index].get();
	}

	inline const EnumValue* Enumeration::GetEnumValue(std::string_view name) const
	{
		for (const auto& enumValue : m_enumValues)
		{
			if (enumValue && enumValue->GetName() == name)
				return enumValue.get();
		}

		CCT_ASSERT_FALSE("Could not find enum value with name: {}", name);
		return nullptr;
	}

	inline const EnumValue* Enumeration::GetEnumValue(cct::Int64 value) const
	{
		for (const auto& enumValue : m_enumValues)
		{
			if (enumValue && enumValue->GetValue() == value)
				return enumValue.get();
		}

		CCT_ASSERT_FALSE("Could not find enum value with value: {}", value);
		return nullptr;
	}

	inline cct::Int64 Enumeration::GetValue() const
	{
		return m_value;
	}
} // namespace cct::refl
