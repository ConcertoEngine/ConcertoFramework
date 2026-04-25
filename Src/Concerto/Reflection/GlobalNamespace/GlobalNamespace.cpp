//
// Created by arthur on 11/12/2024.
//

#include "Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp"

#include <ranges>
#include <set>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"
#include "Concerto/Reflection/Namespace/Namespace.hpp"

namespace cct::refl
{
	std::unique_ptr<GlobalNamespace> GlobalNamespace::m_globalNamespace = std::make_unique<GlobalNamespace>();

	GlobalNamespace& GlobalNamespace::Get()
	{
		CCT_ASSERT(m_globalNamespace, "Invalid pointer");
		return *m_globalNamespace;
	}

	std::size_t GlobalNamespace::GetNamespaceCount() const
	{
		std::set<std::string_view> namespaces;
		for (auto& ns : m_namespaces)
			namespaces.emplace(ns->GetName());
		return namespaces.size();
	}

	std::size_t GlobalNamespace::GetClassCount() const
	{
		std::size_t count = m_classes.size();
		for (auto* ns : m_namespaces)
		{
			count += ns->GetClassCount();
		}
		return count;
	}

	const Class* GlobalNamespace::GetClassByName(std::string_view name) const
	{
		for (const Class* klass : m_classes)
		{
			if (klass->GetName() == name)
				return klass;
		}
		return nullptr;
	}

	Namespace* GlobalNamespace::GetNamespaceByName(std::string_view name) const
	{
		using namespace std::string_view_literals;
		if (name.empty())
			return nullptr;

		if (name == "::"sv)
			name = name.substr(2);

		using namespace std::string_view_literals;
		auto split = (name.starts_with("::"sv) ? name.substr(2) : name) | std::ranges::views::split("::"sv);

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
		{
			for (auto* ns : m_namespaces)
			{
				if (ns->GetName() == res[0])
					return ns;
			}
		}

		return GetNamespaceByName(res);
	}

	Namespace* GlobalNamespace::GetNamespaceByName(std::span<std::string_view> names) const
	{
		using namespace std::string_view_literals;
		if (names.empty())
			return nullptr;
		if (names[0] == "::"sv)
			names = names.subspan(1);

		for (auto* ns : m_namespaces)
		{
			if (ns->GetName() == names[0])
			{
				if (names.size() == 1)
					return ns;
				Namespace* nestedNs = ns->GetNamespace(names.subspan(1));
				if (nestedNs)
					return nestedNs;
			}
		}
		return nullptr;
	}

	void GlobalNamespace::LoadNamespaces() const
	{
		for (auto& nameSpace : m_namespaces)
		{
			nameSpace->LoadNamespaces();
		}
	}

	void GlobalNamespace::LoadClasses() const
	{
		for (auto& nameSpace : m_namespaces)
		{
			CCT_ASSERT(nameSpace, "Invalid namespace pointer");
			nameSpace->LoadClasses();
		}
	}

	void GlobalNamespace::InitializeClasses() const
	{
		for (auto& nameSpace : m_namespaces)
		{
			CCT_ASSERT(nameSpace, "Invalid namespace pointer");
			nameSpace->InitializeClasses();
		}
	}

	void GlobalNamespace::AddNamespace(Namespace* namespace_)
	{
		m_namespaces.push_back(namespace_);
	}

	void GlobalNamespace::AddClass(const Class* klass)
	{
		m_classes.push_back(klass);
	}

	void GlobalNamespace::RemoveNamespace(std::string_view name)
	{
		std::erase_if(m_namespaces, [&](const Namespace* ns)
					  { return ns->GetName() == name; });
	}

	void GlobalNamespace::RemoveClass(std::string_view name)
	{
		std::erase_if(m_classes, [&](const Class* ns)
					  { return ns->GetName() == name; });
	}
} // namespace cct::refl
