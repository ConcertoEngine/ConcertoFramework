#include "Concerto/Reflection/UInt64/UInt64.refl.hpp"

namespace cct::refl
{
	UInt64::UInt64(cct::UInt64 value) :
		m_value(value)
	{
	}

	UInt64::UInt64(const UInt64& other) :
		Object(other),
		m_value(other.m_value)
	{
	}
	UInt64::UInt64(UInt64&& other) noexcept :
		Object(std::move(other)),
		m_value(other.m_value)
	{
	}

	UInt64& UInt64::operator=(const UInt64& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	UInt64& UInt64::operator=(UInt64&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void UInt64::Set(cct::UInt64 value)
	{
		if (m_value == value)
			return;
		m_value = value;
		OnValueChanged.Emit();
	}

	cct::UInt64 UInt64::Get() const
	{
		return m_value;
	}
	UInt64::operator cct::UInt64() const
	{
		return m_value;
	}

	UInt64& UInt64::operator=(cct::UInt64 value)
	{
		Set(value);
		return *this;
	}

	bool UInt64::operator==(const UInt64& other) const
	{
		return m_value == other.m_value;
	}
	bool UInt64::operator!=(const UInt64& other) const
	{
		return m_value != other.m_value;
	}
	bool UInt64::operator==(cct::UInt64 other) const
	{
		return m_value == other;
	}
	bool UInt64::operator!=(cct::UInt64 other) const
	{
		return m_value != other;
	}
	std::string UInt64::ToString() const
	{
		return std::to_string(m_value);
	}
} // namespace cct::refl
