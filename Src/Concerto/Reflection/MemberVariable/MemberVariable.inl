//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_MEMBERVARIABLE_INL
#define CONCERTO_REFLECTION_MEMBERVARIABLE_INL

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"

namespace cct::refl
{
	inline std::string_view MemberVariable::GetName() const
	{
		return m_name;
	}

	inline std::size_t MemberVariable::GetIndex() const
	{
		return m_index;
	}

	inline const Class* MemberVariable::GetType() const
	{
		CCT_ASSERT(m_type, "MemberVariable has no type");
		return m_type;
	}

	inline NativeMemberVariable::NativeMemberVariable(std::string name, UInt64 typeId, std::size_t index) :
		m_name(std::move(name)),
		m_typeId(typeId),
		m_index(index)
	{
	}

	inline std::string_view NativeMemberVariable::GetName() const
	{
		return m_name;
	}

	inline std::size_t NativeMemberVariable::GetIndex() const
	{
		return m_index;
	}

	inline UInt64 NativeMemberVariable::GetTypeId() const
	{
		return m_typeId;
	}
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_MEMBERVARIABLE_INL