//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_NAMESPACE_INL
#define CONCERTO_REFLECTION_NAMESPACE_INL

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/Namespace/Namespace.hpp"

namespace cct::refl
{
	inline Namespace::Namespace(std::string name) :
		m_name(std::move(name)),
		m_hash(std::hash<std::string>()(name))
	{
	}

	inline std::string_view Namespace::GetName() const
	{
		return m_name;
	}

	inline std::size_t Namespace::GetHash() const
	{
		return m_hash;
	}

	inline std::size_t Namespace::GetClassCount() const
	{
		std::size_t count = m_classes.size();
		for (auto& ns : m_namespaces)
			count += ns->GetClassCount();
		return count;
	}

	inline std::size_t Namespace::GetNamespaceCount() const
	{
		return m_namespaces.size();
	}

	inline const Class* Namespace::GetClass(std::size_t index) const
	{
		if (index > GetClassCount())
			return nullptr;
		return m_classes[index].get();
	}

	inline bool Namespace::HasClass(std::string_view name) const
	{
		return GetClass(name) != nullptr;
	}
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_NAMESPACE_INL