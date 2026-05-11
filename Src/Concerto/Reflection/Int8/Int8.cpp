#include "Concerto/Reflection/Int8/Int8.refl.hpp"

namespace cct::refl
{
	Int8::Int8(cct::Int8 value) :
		m_value(value)
	{
	}

	Int8::Int8(const Int8& other) :
		Object(other),
		m_value(other.m_value)
	{
	}
	Int8::Int8(Int8&& other) noexcept :
		Object(std::move(other)),
		m_value(other.m_value)
	{
	}

	Int8& Int8::operator=(const Int8& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	Int8& Int8::operator=(Int8&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void Int8::Set(cct::Int8 value)
	{
		if (m_value == value)
			return;
		m_value = value;
		OnValueChanged.Emit();
	}

	cct::Int8 Int8::Get() const
	{
		return m_value;
	}
	Int8::operator cct::Int8() const
	{
		return m_value;
	}

	Int8& Int8::operator=(cct::Int8 value)
	{
		Set(value);
		return *this;
	}

	bool Int8::operator==(const Int8& other) const
	{
		return m_value == other.m_value;
	}
	bool Int8::operator!=(const Int8& other) const
	{
		return m_value != other.m_value;
	}
	bool Int8::operator==(cct::Int8 other) const
	{
		return m_value == other;
	}
	bool Int8::operator!=(cct::Int8 other) const
	{
		return m_value != other;
	}
	std::string Int8::ToString() const
	{
		return std::to_string(static_cast<int>(m_value));
	}
} // namespace cct::refl
