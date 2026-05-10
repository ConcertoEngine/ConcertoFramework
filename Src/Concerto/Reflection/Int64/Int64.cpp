#include "Concerto/Reflection/Int64/Int64.refl.hpp"

namespace cct::refl
{
	Int64::Int64(cct::Int64 value) : m_value(value) {}

	Int64::Int64(const Int64& other) : Object(other), m_value(other.m_value) {}
	Int64::Int64(Int64&& other) noexcept : Object(std::move(other)), m_value(other.m_value) {}

	Int64& Int64::operator=(const Int64& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	Int64& Int64::operator=(Int64&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void Int64::Set(cct::Int64 value)
	{
		if (m_value == value) return;
		m_value = value;
		OnValueChanged.Emit();
	}

	cct::Int64 Int64::Get() const { return m_value; }
	Int64::operator cct::Int64() const { return m_value; }

	Int64& Int64::operator=(cct::Int64 value) { Set(value); return *this; }

	bool Int64::operator==(const Int64& other) const { return m_value == other.m_value; }
	bool Int64::operator!=(const Int64& other) const { return m_value != other.m_value; }
	bool Int64::operator==(cct::Int64 other) const { return m_value == other; }
	bool Int64::operator!=(cct::Int64 other) const { return m_value != other; }
	std::string Int64::ToString() const { return std::to_string(m_value); }
} // namespace cct::refl
