#include "Concerto/Reflection/UInt32/UInt32.refl.hpp"

namespace cct::refl
{
	UInt32::UInt32(cct::UInt32 value) :
		m_value(value)
	{
	}

	UInt32::UInt32(const UInt32& other) :
		Object(other),
		m_value(other.m_value)
	{
	}
	UInt32::UInt32(UInt32&& other) noexcept :
		Object(std::move(other)),
		m_value(other.m_value)
	{
	}

	UInt32& UInt32::operator=(const UInt32& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	UInt32& UInt32::operator=(UInt32&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void UInt32::Set(cct::UInt32 value)
	{
		if (m_value == value)
			return;
		m_value = value;
		OnValueChanged.Emit();
	}

	cct::UInt32 UInt32::Get() const
	{
		return m_value;
	}
	UInt32::operator cct::UInt32() const
	{
		return m_value;
	}

	UInt32& UInt32::operator=(cct::UInt32 value)
	{
		Set(value);
		return *this;
	}

	bool UInt32::operator==(const UInt32& other) const
	{
		return m_value == other.m_value;
	}
	bool UInt32::operator!=(const UInt32& other) const
	{
		return m_value != other.m_value;
	}
	bool UInt32::operator==(cct::UInt32 other) const
	{
		return m_value == other;
	}
	bool UInt32::operator!=(cct::UInt32 other) const
	{
		return m_value != other;
	}
	std::string UInt32::ToString() const
	{
		return std::to_string(m_value);
	}
} // namespace cct::refl
