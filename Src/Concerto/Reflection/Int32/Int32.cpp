#include "Concerto/Reflection/Int32/Int32.refl.hpp"

namespace cct::refl
{
	Int32::Int32(cct::Int32 value) : m_value(value) {}

	Int32::Int32(const Int32& other) : Object(other), m_value(other.m_value) {}
	Int32::Int32(Int32&& other) noexcept : Object(std::move(other)), m_value(other.m_value) {}

	Int32& Int32::operator=(const Int32& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	Int32& Int32::operator=(Int32&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void Int32::Set(cct::Int32 value)
	{
		if (m_value == value) return;
		m_value = value;
		OnValueChanged.Emit();
	}

	cct::Int32 Int32::Get() const { return m_value; }
	Int32::operator cct::Int32() const { return m_value; }

	Int32& Int32::operator=(cct::Int32 value) { Set(value); return *this; }

	bool Int32::operator==(const Int32& other) const { return m_value == other.m_value; }
	bool Int32::operator!=(const Int32& other) const { return m_value != other.m_value; }
	bool Int32::operator==(cct::Int32 other) const { return m_value == other; }
	bool Int32::operator!=(cct::Int32 other) const { return m_value != other; }
} // namespace cct::refl
