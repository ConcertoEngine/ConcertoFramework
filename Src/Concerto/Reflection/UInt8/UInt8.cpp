#include "Concerto/Reflection/UInt8/UInt8.refl.hpp"

namespace cct::refl
{
	UInt8::UInt8(cct::UInt8 value) :
		m_value(value)
	{
	}

	UInt8::UInt8(const UInt8& other) :
		Object(other),
		m_value(other.m_value)
	{
	}
	UInt8::UInt8(UInt8&& other) noexcept :
		Object(std::move(other)),
		m_value(other.m_value)
	{
	}

	UInt8& UInt8::operator=(const UInt8& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	UInt8& UInt8::operator=(UInt8&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void UInt8::Set(cct::UInt8 value)
	{
		if (m_value == value)
			return;
		m_value = value;
		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnValueChanged.Emit();
		}
	}

	cct::UInt8 UInt8::Get() const
	{
		return m_value;
	}
	UInt8::operator cct::UInt8() const
	{
		return m_value;
	}

	UInt8& UInt8::operator=(cct::UInt8 value)
	{
		Set(value);
		return *this;
	}

	bool UInt8::operator==(const UInt8& other) const
	{
		return m_value == other.m_value;
	}
	bool UInt8::operator!=(const UInt8& other) const
	{
		return m_value != other.m_value;
	}
	bool UInt8::operator==(cct::UInt8 other) const
	{
		return m_value == other;
	}
	bool UInt8::operator!=(cct::UInt8 other) const
	{
		return m_value != other;
	}
	std::string UInt8::ToString() const
	{
		return std::to_string(static_cast<unsigned>(m_value));
	}
} // namespace cct::refl
