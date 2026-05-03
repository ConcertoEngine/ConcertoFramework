#include "Concerto/Reflection/Boolean/Boolean.refl.hpp"

namespace cct::refl
{
	Boolean::Boolean(bool value) :
		m_value(value)
	{
	}

	Boolean::Boolean(const Boolean& other) :
		Object(other),
		m_value(other.m_value)
	{
	}

	Boolean::Boolean(Boolean&& other) noexcept :
		Object(std::move(other)),
		m_value(other.m_value)
	{
	}

	Boolean& Boolean::operator=(const Boolean& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	Boolean& Boolean::operator=(Boolean&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void Boolean::Set(bool value)
	{
		if (m_value == value)
			return;
		m_value = value;
		OnValueChanged.Emit();
	}

	bool Boolean::Get() const
	{
		return m_value;
	}

	Boolean::operator bool() const
	{
		return m_value;
	}

	Boolean& Boolean::operator=(bool value)
	{
		Set(value);
		return *this;
	}

	bool Boolean::operator==(const Boolean& other) const
	{
		return m_value == other.m_value;
	}

	bool Boolean::operator!=(const Boolean& other) const
	{
		return m_value != other.m_value;
	}

	bool Boolean::operator==(bool other) const
	{
		return m_value == other;
	}

	bool Boolean::operator!=(bool other) const
	{
		return m_value != other;
	}
} // namespace cct::refl
