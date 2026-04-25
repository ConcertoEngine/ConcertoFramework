//
// Created by arthur on 10/11/2024.
//

#ifndef CONCERTO_REFLECTION_METHOD_INL
#define CONCERTO_REFLECTION_METHOD_INL

#include "Concerto/Reflection/Method/Method.hpp"

namespace cct::refl
{
	inline std::string_view Method::GetName() const
	{
		return m_name;
	}

	inline const Class* Method::GetReturnValue() const
	{
		return m_returnValue;
	}

	inline std::vector<const Class*> Method::GetParameters() const
	{
		return m_parameters;
	}

	inline std::size_t Method::GetIndex() const
	{
		return m_index;
	}

	inline bool Method::HasAttribute(std::string_view attribute) const
	{
		// not using "contains", because it does not support std::string_view
		auto it = std::find_if(m_attributes.begin(), m_attributes.end(), [&](const std::pair<std::string, std::string>& value) -> bool
							   { return "attribute" == value.first; });
		return it != m_attributes.end();
	}

	inline std::string_view Method::GetAttribute(std::string_view attribute) const
	{
		// not using "contains", because it does not support std::string_view
		auto it = std::find_if(m_attributes.begin(), m_attributes.end(), [&](const std::pair<std::string, std::string>& value) -> bool
							   { return "attribute" == value.first; });
		if (it == m_attributes.end())
		{
			CCT_ASSERT_FALSE("Attribute '{}' does not exist", attribute);
			return {};
		}
		return it->second;
	}

	inline void Method::AddAttribute(std::string name, std::string value)
	{
		CCT_ASSERT(HasAttribute(name), "Class attribute already exist");
		m_attributes.emplace(std::move(name), std::move(value));
	}

	inline void* Method::GetCustomDelegate() const
	{
		return m_customDelegate;
	}

	inline void Method::SetCustomDelegate(void* delegate)
	{
		m_customDelegate = delegate;
	}
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_METHOD_INL