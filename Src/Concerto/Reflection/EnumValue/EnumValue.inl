//
// Created by arthur on 31/12/2025
//

#pragma once

namespace cct::refl
{
	inline std::string_view EnumValue::GetName() const
	{
		return m_name;
	}

	inline cct::Int64 EnumValue::GetValue() const
	{
		return m_value;
	}

	inline std::size_t EnumValue::GetIndex() const
	{
		return m_index;
	}
} // namespace cct::refl
