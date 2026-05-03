#include "Concerto/Reflection/Int16/Int16.refl.hpp"

namespace cct::refl
{
	Int16::Int16(cct::Int16 value) : m_value(value) {}

	Int16::Int16(const Int16& other) : Object(other), m_value(other.m_value) {}
	Int16::Int16(Int16&& other) noexcept : Object(std::move(other)), m_value(other.m_value) {}

	Int16& Int16::operator=(const Int16& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	Int16& Int16::operator=(Int16&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void Int16::Set(cct::Int16 value)
	{
		if (m_value == value) return;
		m_value = value;
		OnValueChanged.Emit();
	}

	cct::Int16 Int16::Get() const { return m_value; }
	Int16::operator cct::Int16() const { return m_value; }

	Int16& Int16::operator=(cct::Int16 value) { Set(value); return *this; }

	bool Int16::operator==(const Int16& other) const { return m_value == other.m_value; }
	bool Int16::operator!=(const Int16& other) const { return m_value != other.m_value; }
	bool Int16::operator==(cct::Int16 other) const { return m_value == other; }
	bool Int16::operator!=(cct::Int16 other) const { return m_value != other; }
} // namespace cct::refl
