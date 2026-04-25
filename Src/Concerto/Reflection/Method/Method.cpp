//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Method/Method.hpp"

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"

namespace cct::refl
{
	Method::Method(std::string_view name, const Class* returnValue, std::vector<const Class*> parameters, std::size_t index, void* delegate) :
		m_name(name),
		m_returnValue(returnValue),
		m_parameters(std::move(parameters)),
		m_index(index),
		m_customDelegate(delegate)
	{
	}
} // namespace cct::refl
