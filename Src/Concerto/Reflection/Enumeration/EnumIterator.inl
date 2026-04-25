//
// Created by arthur on 31/12/2025
//

#pragma once

#include "Concerto/Reflection/Enumeration/EnumIterator.hpp"
#include "Concerto/Reflection/EnumValue/EnumValue.hpp"

namespace cct::refl
{
	inline std::string_view EnumIterator::GetName() const
	{
		if (m_index >= m_enumValues.size() || !m_enumValues[m_index])
			return {};
		return m_enumValues[m_index]->GetName();
	}

	inline cct::Int64 EnumIterator::GetValue() const
	{
		if (m_index >= m_enumValues.size() || !m_enumValues[m_index])
			return 0;
		return m_enumValues[m_index]->GetValue();
	}

	inline std::size_t EnumIterator::GetIndex() const
	{
		if (m_index >= m_enumValues.size() || !m_enumValues[m_index])
			return 0;
		return m_enumValues[m_index]->GetIndex();
	}

	inline EnumIterator& EnumIterator::operator++()
	{
		++m_index;
		return *this;
	}

	inline bool EnumIterator::operator!=(const EnumIterator& other) const
	{
		return m_index != other.m_index;
	}
} // namespace cct::refl
