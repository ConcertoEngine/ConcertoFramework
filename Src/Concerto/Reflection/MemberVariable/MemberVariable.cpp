//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"

namespace cct::refl
{
	MemberVariable::MemberVariable(std::string name, const Class* type, std::size_t index) :
		m_name(std::move(name)),
		m_index(index),
		m_type(type)
	{
	}
} // namespace cct::refl
