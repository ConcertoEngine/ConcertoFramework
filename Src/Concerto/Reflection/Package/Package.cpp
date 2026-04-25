//
// Created by arthur on 10/11/2024.
//

#include "Concerto/Reflection/Package/Package.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/GlobalNamespace/GlobalNamespace.hpp"
#include "Concerto/Reflection/MemberVariable/MemberVariable.hpp"
#include "Concerto/Reflection/Method/Method.hpp"
#include "Concerto/Reflection/Namespace/Namespace.hpp"

namespace cct::refl
{
	Package::Package(std::string name) :
		m_name(std::move(name))
	{
	}

	Namespace* Package::GetNamespace(std::size_t index) const
	{
		if (index > m_namespaces.size())
			return nullptr;
		return m_namespaces[index].get();
	}

	Namespace* Package::GetNamespace(std::string_view name) const
	{
		auto it = std::find_if(m_namespaces.begin(), m_namespaces.end(), [&](const std::unique_ptr<Namespace>& value) -> bool
							   { return value->GetName() == name; });

		if (it != m_namespaces.end())
			return it->get();
		return nullptr;
	}

	void Package::AddNamespace(std::unique_ptr<Namespace> nameSpace)
	{
		if (nameSpace == nullptr)
		{
			CCT_ASSERT_FALSE("Namespace is null");
			return;
		}
		GlobalNamespace::Get().AddNamespace(nameSpace.get());
		m_namespaces.emplace_back(std::move(nameSpace));
	}

	void Package::AddClass(std::unique_ptr<Class> klass)
	{
		GlobalNamespace::Get().AddClass(klass.get());
		m_classes.push_back(std::move(klass));
	}
} // namespace cct::refl
