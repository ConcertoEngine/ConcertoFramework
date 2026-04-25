//
// Created by arthur on 27/02/2026.
//

#include "Concerto/Reflection/String/String.refl.hpp"

namespace cct::refl
{
	String::String(std::string_view value) :
		m_value(value)
	{
	}

	String::String(std::string value) :
		m_value(std::move(value))
	{
	}

	String::String(const String& other) :
		Object(other),
		m_value(other.m_value)
		// OnValueChanged connections are NOT copied (Signal copy semantics)
	{
	}

	String::String(String&& other) noexcept :
		Object(std::move(other)),
		m_value(std::move(other.m_value))
		// OnValueChanged connections follow the Signal move
	{
	}

	String& String::operator=(const String& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
			// OnValueChanged connections are NOT propagated on assignment
		}
		return *this;
	}

	String& String::operator=(String&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = std::move(other.m_value);
		}
		return *this;
	}

	void String::Set(std::string_view value)
	{
		m_value = value;
		OnValueChanged.Emit();
	}

	void String::Set(std::string value)
	{
		m_value = std::move(value);
		OnValueChanged.Emit();
	}

	const std::string& String::Get() const
	{
		return m_value;
	}

	std::string& String::Get()
	{
		return m_value;
	}

	String::operator std::string_view() const
	{
		return m_value;
	}

	String& String::operator=(std::string_view value)
	{
		Set(value);
		return *this;
	}

	String& String::operator=(std::string value)
	{
		Set(std::move(value));
		return *this;
	}

	bool String::operator==(const String& other) const
	{
		return m_value == other.m_value;
	}

	bool String::operator!=(const String& other) const
	{
		return m_value != other.m_value;
	}

	bool String::operator==(std::string_view other) const
	{
		return m_value == other;
	}

	bool String::operator!=(std::string_view other) const
	{
		return m_value != other;
	}
} // namespace cct::refl
