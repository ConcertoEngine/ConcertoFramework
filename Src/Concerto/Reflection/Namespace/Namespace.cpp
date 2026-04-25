//
// Created by arthur on 10/11/2024.
//
#include "Concerto/Reflection/Namespace/Namespace.hpp"

#include <ranges>
#include <string>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"

namespace cct::refl
{
	using namespace std::string_literals;

	const Class* Namespace::GetClass(std::string_view name) const
	{
		using namespace std::string_view_literals;
		if (name.find("::"sv) != std::string_view::npos)
		{
			using namespace std::string_view_literals;
			auto split = name | std::ranges::views::split("::"sv);

			std::vector<std::string_view> res;
			for (const auto elem : split)
				res.emplace_back(elem.data(), elem.size());

			if (res.size() >= 1 && res[0] == ""sv)
				res.erase(res.begin()); // because split on "::" gives an empty first element

			if (res.size() > 1)
			{
				for (auto& nameSpace : m_namespaces)
				{
					if (nameSpace->GetName() != res[0])
						continue;
					auto klass = nameSpace->GetClass(name.substr(res[0].size() + 2 /*2 because of '::'*/));
					if (klass != nullptr)
						return klass;
				}
				GetClassByName(name);
			}
		}

		auto it = std::find_if(m_classes.begin(), m_classes.end(), [&](const std::unique_ptr<Class>& value) -> bool
							   { return value->GetName() == name; });
		if (it != m_classes.end())
			return it->get();
		return nullptr;
	}

	inline Namespace* Namespace::GetNamespace(std::size_t index) const
	{
		if (index > GetNamespaceCount())
			return nullptr;
		return m_namespaces[index].get();
	}

	void Namespace::AddClass(std::unique_ptr<Class> klass)
	{
		m_classes.emplace_back(std::move(klass));
	}

	void Namespace::AddNamespace(std::unique_ptr<Namespace> nameSpace)
	{
		m_namespaces.emplace_back(std::move(nameSpace));
	}

	Namespace* Namespace::GetNamespace(std::string_view name) const
	{
		for (auto& nameSpace : m_namespaces)
		{
			if (nameSpace->GetName() == name)
				return nameSpace.get();
		}
		return nullptr;
	}

	Namespace* Namespace::GetNamespace(std::span<std::string_view> namespaces) const
	{
		if (namespaces.empty())
		{
			CCT_ASSERT_FALSE("Empty list");
			return nullptr;
		}
		auto nameSpace = GetNamespace(namespaces[0]);
		if (nameSpace == nullptr)
			return nullptr;
		auto res = namespaces.subspan(1);
		if (res.empty())
			return nameSpace;
		return nameSpace->GetNamespace(res);
	}
} // namespace cct::refl
