//
// Created by arthur on 31/12/2025
//

#include "Concerto/Reflection/EnumValue/EnumValue.hpp"

#include "Concerto/Core/Assert.hpp"

namespace cct::refl
{
	EnumValue::EnumValue(std::string name, cct::Int64 value, std::size_t index) :
		m_name(std::move(name)),
		m_value(value),
		m_index(index)
	{
	}

	bool EnumValue::HasAttribute(std::string_view attribute) const
	{
		return m_attributes.find(std::string(attribute)) != m_attributes.end();
	}

	std::string_view EnumValue::GetAttribute(std::string_view attribute) const
	{
		const auto it = m_attributes.find(std::string(attribute));
		if (it != m_attributes.end())
			return it->second;

		CCT_ASSERT_FALSE("EnumValue::GetAttribute: attribute '{}' not found", attribute);
		return "";
	}

	void EnumValue::AddAttribute(std::string name, std::string value)
	{
		m_attributes[std::move(name)] = std::move(value);
	}
} // namespace cct::refl
