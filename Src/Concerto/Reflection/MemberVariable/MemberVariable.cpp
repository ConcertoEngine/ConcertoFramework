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

	bool MemberVariable::HasAttribute(std::string_view name) const
	{
		return m_attributes.find(std::string(name)) != m_attributes.end();
	}

	std::string_view MemberVariable::GetAttribute(std::string_view name) const
	{
		auto it = m_attributes.find(std::string(name));
		if (it == m_attributes.end())
			return {};
		return it->second;
	}

	void MemberVariable::AddAttribute(std::string name, std::string value)
	{
		m_attributes.emplace(std::move(name), std::move(value));
	}

	const std::unordered_map<std::string, std::string>& MemberVariable::GetAttributes() const
	{
		return m_attributes;
	}

	bool NativeMemberVariable::HasAttribute(std::string_view name) const
	{
		return m_attributes.find(std::string(name)) != m_attributes.end();
	}

	std::string_view NativeMemberVariable::GetAttribute(std::string_view name) const
	{
		auto it = m_attributes.find(std::string(name));
		if (it == m_attributes.end())
			return {};
		return it->second;
	}

	void NativeMemberVariable::AddAttribute(std::string name, std::string value)
	{
		m_attributes.emplace(std::move(name), std::move(value));
	}

	const std::unordered_map<std::string, std::string>& NativeMemberVariable::GetAttributes() const
	{
		return m_attributes;
	}
} // namespace cct::refl
