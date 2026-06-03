//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Class/Class.hpp"

#include <ranges>
#include <string_view>

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"
#include "Concerto/Reflection/Namespace/Namespace.hpp"

namespace cct::refl
{
	Class::Class(Namespace* nameSpace, std::string name, const Class* baseClass) :
		m_name(std::move(name)),
		m_namespace(nameSpace),
		m_baseClass(baseClass ? baseClass : nullptr),
		m_hash(0)
	{
		m_hash = m_namespace ? m_namespace->GetHash() : 0;
		m_hash |= m_baseClass ? m_baseClass->GetHash() : 0;
		m_hash |= std::hash<std::string>()(m_name);
	}

	std::string_view Class::GetName() const
	{
		return m_name;
	}

	std::string Class::GetFullyQualifiedName() const
	{
		if (!m_namespace)
			return m_name;
		std::string ns = m_namespace->GetFullName();
		if (ns.empty())
			return m_name;
		return ns + "::" + m_name;
	}

	std::string_view Class::GetNamespaceName() const
	{
		if (m_namespace == nullptr)
		{
			CCT_ASSERT_FALSE("Invalid namespace pointer");
			return {};
		}
		return m_namespace->GetName();
	}

	const Namespace& Class::GetNamespace() const
	{
		if (m_namespace == nullptr)
		{
			CCT_ASSERT_FALSE("Invalid namespace pointer");
			throw std::runtime_error("Invalid namespace pointer");
		}
		return *m_namespace;
	}

	std::size_t Class::GetHash() const
	{
		return m_hash;
	}

	std::size_t Class::GetMemberVariableCount() const
	{
		return m_memberVariables.size();
	}

	std::size_t Class::GetNativeMemberVariableCount() const
	{
		return m_nativeMemberVariables.size();
	}

	std::size_t Class::GetTotalNativeMemberCount() const
	{
		std::size_t baseCount = m_baseClass ? m_baseClass->GetTotalNativeMemberCount() : 0;
		return baseCount + m_nativeMemberVariables.size();
	}

	std::size_t Class::GetMethodCount() const
	{
		return m_methods.size();
	}

	const Class* Class::GetBaseClass() const
	{
		return m_baseClass;
	}

	std::span<std::unique_ptr<MemberVariable>> Class::GetMemberVariables()
	{
		return m_memberVariables;
	}

	std::span<const std::unique_ptr<MemberVariable>> Class::GetMemberVariables() const
	{
		return m_memberVariables;
	}

	std::span<std::unique_ptr<NativeMemberVariable>> Class::GetNativeMemberVariables()
	{
		return m_nativeMemberVariables;
	}

	std::span<const std::unique_ptr<NativeMemberVariable>> Class::GetNativeMemberVariables() const
	{
		return m_nativeMemberVariables;
	}

	std::span<std::unique_ptr<Method>> Class::GetMethods()
	{
		return m_methods;
	}

	std::span<const std::unique_ptr<Method>> Class::GetMethods() const
	{
		return m_methods;
	}

	const MemberVariable* Class::GetMemberVariable(std::size_t index) const
	{
		if (m_memberVariables.empty())
			return nullptr;
		if (index > m_memberVariables.size())
			return nullptr;
		return m_memberVariables[index].get();
	}

	const MemberVariable* Class::GetMemberVariable(std::string_view name) const
	{
		for (const auto& variable : m_memberVariables)
		{
			if (variable->GetName() == name)
				return variable.get();
		}
		return m_baseClass ? m_baseClass->GetMemberVariable(name) : nullptr;
	}

	const NativeMemberVariable* Class::GetNativeMemberVariable(std::size_t index) const
	{
		const std::size_t baseCount = m_baseClass != nullptr ? m_baseClass->GetTotalNativeMemberCount() : 0;
		if (index < baseCount)
			return m_baseClass != nullptr ? m_baseClass->GetNativeMemberVariable(index) : nullptr;
		const std::size_t localPos = index - baseCount;
		if (localPos >= m_nativeMemberVariables.size())
			return nullptr;
		return m_nativeMemberVariables[localPos].get();
	}

	const NativeMemberVariable* Class::GetNativeMemberVariable(std::string_view name) const
	{
		for (const auto& variable : m_nativeMemberVariables)
		{
			if (variable->GetName() == name)
				return variable.get();
		}
		// Walk base class chain so inherited native members are found.
		if (m_baseClass != nullptr)
			return m_baseClass->GetNativeMemberVariable(name);
		return nullptr;
	}

	cct::refl::Object* Class::GetMemberVariable(std::string_view name, const cct::refl::Object& self) const
	{
		for (const auto& variable : m_memberVariables)
		{
			if (variable->GetName() == name)
				return GetMemberVariable(variable->GetIndex(), self);
		}

		if (m_baseClass != nullptr)
			return m_baseClass->GetMemberVariable(name, self);
		return nullptr;
	}

	void* Class::GetNativeMemberVariable(std::string_view name, const cct::refl::Object& self) const
	{
		for (const auto& variable : m_nativeMemberVariables)
		{
			if (variable->GetName() == name)
				return GetNativeMemberVariable(variable->GetIndex(), self);
		}
		if (m_baseClass != nullptr)
			return m_baseClass->GetNativeMemberVariable(name, self);
		return nullptr;
	}

	const Method* Class::GetMethod(std::size_t index) const
	{
		if (m_methods.empty())
			return nullptr;
		if (index > m_methods.size())
			return nullptr;
		return m_methods[index].get();
	}

	const Method* Class::GetMethod(std::string_view name) const
	{
		for (const auto& method : m_methods)
		{
			if (method->GetName() == name)
				return method.get();
		}
		return nullptr;
	}

	bool Class::HasMemberVariable(std::string_view name) const
	{
		for (const auto& variable : m_memberVariables)
			if (variable->GetName() == name)
				return true;
		if (m_baseClass != nullptr)
			return m_baseClass->HasMemberVariable(name);
		return false;
	}

	bool Class::HasMethod(std::string_view name) const
	{
		return GetMethod(name) != nullptr;
	}

	bool Class::InheritsFrom(const Class& other) const
	{
		if (!m_baseClass)
		{
			CCT_ASSERT(m_name == "Object", "Invalid base class, it should at least derive from cct::refl::Object");
			return false;
		}
		if (other == *m_baseClass)
			return true;
		return m_baseClass->InheritsFrom(other);
	}

	bool Class::InheritsFrom(std::string_view name) const
	{
		if (!m_baseClass)
		{
			CCT_ASSERT_FALSE("Invalid base class, it should at least derive from cct::refl::Object");
			return false;
		}
		if (m_baseClass->GetName() == name)
			return true;
		return m_baseClass->InheritsFrom(name);
	}

	bool Class::HasAttribute(std::string_view attribute) const
	{
		// not using "contains", because it does not support std::string_view
		auto it = std::find_if(m_attributes.begin(), m_attributes.end(), [&](const std::pair<std::string, std::string>& value) -> bool
							   { return attribute == value.first; });
		return it != m_attributes.end();
	}

	std::string_view Class::GetAttribute(std::string_view attribute) const
	{
		// not using "contains", because it does not support std::string_view
		auto it = std::find_if(m_attributes.begin(), m_attributes.end(), [&](const std::pair<std::string, std::string>& value) -> bool
							   { return attribute == value.first; });
		if (it == m_attributes.end())
		{
			CCT_ASSERT_FALSE("Attribute '{}' does not exist", attribute);
			return {};
		}
		return it->second;
	}

	bool Class::operator==(const Class& other) const
	{
		return GetHash() == other.GetHash();
	}

	bool Class::operator!=(const Class& other) const
	{
		if (this == nullptr)
			return true;
		return !(*this == other);
	}

	bool Class::IsTemplateClass() const
	{
		return false;
	}

	MemberVariable* Class::AddMemberVariable(std::string_view name, const Class* type)
	{
		CCT_ASSERT(!GetMemberVariable(name), "Member variable already exists");
		m_memberVariables.emplace_back(std::make_unique<MemberVariable>(std::string(name), type, m_memberVariables.size()));
		return m_memberVariables.back().get();
	}

	NativeMemberVariable* Class::AddNativeMemberVariable(std::string_view name, UInt64 typeId)
	{
		CCT_ASSERT(!GetNativeMemberVariable(name), "Member variable already exists");
		std::size_t baseCount = m_baseClass ? m_baseClass->GetTotalNativeMemberCount() : 0;
		m_nativeMemberVariables.emplace_back(std::make_unique<NativeMemberVariable>(std::string(name), typeId, baseCount + m_nativeMemberVariables.size()));
		return m_nativeMemberVariables.back().get();
	}

	void Class::AddMemberFunction(std::unique_ptr<Method> method)
	{
		CCT_ASSERT(!GetMethod(method->GetName()), "Method already registered");
		m_methods.emplace_back(std::move(method));
	}

	void Class::AddAttribute(std::string name, std::string value)
	{
		CCT_ASSERT(HasAttribute(name), "Class attribute already exist");
		m_attributes.emplace(std::move(name), std::move(value));
	}

	void Class::SetNamespace(Namespace* nameSpace)
	{
		m_namespace = nameSpace;
	}

	void Class::SetBaseClass(const Class* klass)
	{
		m_baseClass = klass;
	}

	const Class* GetClassByName(std::string_view nameSpaceName, std::string_view name)
	{
		using namespace std::string_view_literals;
		if (nameSpaceName.empty() || nameSpaceName == "::"sv)
			return GlobalNamespace::Get().GetClassByName(name);
		const auto nameSpace = GlobalNamespace::Get().GetNamespaceByName(nameSpaceName);
		if (nameSpace)
			return nameSpace->GetClass(name);
		return nullptr;
	}

	const Class* GetClassByName(std::span<std::string_view> nameSpaceNames, std::string_view name)
	{
		return GlobalNamespace::Get().FindClass(nameSpaceNames, name);
	}

	const Class* GetClassByName(std::string_view name)
	{
		using namespace std::string_view_literals;
		auto split = name | std::ranges::views::split("::"sv);

		std::vector<std::string_view> res;
		for (const auto elem : split)
			res.emplace_back(elem.data(), elem.size());

		if (res.size() >= 1 && res[0] == ""sv)
			res.erase(res.begin()); // because split on "::" gives an empty first element

		if (res.empty())
		{
			CCT_ASSERT_FALSE("Invalid class: {}", name);
			return nullptr;
		}

		if (res.size() == 1)
			return GetClassByName("", res[0]);

		std::string_view klass = res.back();
		res.pop_back();
		return GetClassByName(res, klass);
	}
} // namespace cct::refl
