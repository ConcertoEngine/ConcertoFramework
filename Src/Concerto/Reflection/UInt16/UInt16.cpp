#include "Concerto/Reflection/UInt16/UInt16.refl.hpp"

namespace cct::refl
{
	UInt16::UInt16(cct::UInt16 value) : m_value(value) {}

	UInt16::UInt16(const UInt16& other) : Object(other), m_value(other.m_value) {}
	UInt16::UInt16(UInt16&& other) noexcept : Object(std::move(other)), m_value(other.m_value) {}

	UInt16& UInt16::operator=(const UInt16& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	UInt16& UInt16::operator=(UInt16&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void UInt16::Set(cct::UInt16 value)
	{
		if (m_value == value) return;
		m_value = value;
		OnValueChanged.Emit();
	}

	cct::UInt16 UInt16::Get() const { return m_value; }
	UInt16::operator cct::UInt16() const { return m_value; }

	UInt16& UInt16::operator=(cct::UInt16 value) { Set(value); return *this; }

	bool UInt16::operator==(const UInt16& other) const { return m_value == other.m_value; }
	bool UInt16::operator!=(const UInt16& other) const { return m_value != other.m_value; }
	bool UInt16::operator==(cct::UInt16 other) const { return m_value == other; }
	bool UInt16::operator!=(cct::UInt16 other) const { return m_value != other; }
} // namespace cct::refl
