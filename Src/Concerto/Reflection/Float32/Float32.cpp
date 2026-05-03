#include "Concerto/Reflection/Float32/Float32.refl.hpp"

namespace cct::refl
{
	Float32::Float32(cct::Float32 value) : m_value(value) {}

	Float32::Float32(const Float32& other) : Object(other), m_value(other.m_value) {}
	Float32::Float32(Float32&& other) noexcept : Object(std::move(other)), m_value(other.m_value) {}

	Float32& Float32::operator=(const Float32& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	Float32& Float32::operator=(Float32&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void Float32::Set(cct::Float32 value)
	{
		if (m_value == value) return;
		m_value = value;
		OnValueChanged.Emit();
	}

	cct::Float32 Float32::Get() const { return m_value; }
	Float32::operator cct::Float32() const { return m_value; }

	Float32& Float32::operator=(cct::Float32 value) { Set(value); return *this; }

	bool Float32::operator==(const Float32& other) const { return m_value == other.m_value; }
	bool Float32::operator!=(const Float32& other) const { return m_value != other.m_value; }
	bool Float32::operator==(cct::Float32 other) const { return m_value == other; }
	bool Float32::operator!=(cct::Float32 other) const { return m_value != other; }
} // namespace cct::refl
