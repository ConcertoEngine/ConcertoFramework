#include <Concerto/Core/ThreadAffinity/ThreadAffinity.hpp>
#include <iomanip>
#include <sstream>
#include <string>

#include "Concerto/Reflection/Float64/Float64.refl.hpp"

namespace cct::refl
{
	Float64::Float64(cct::Float64 value) :
		m_value(value)
	{
	}

	Float64::Float64(const Float64& other) :
		Object(other),
		m_value(other.m_value)
	{
	}
	Float64::Float64(Float64&& other) noexcept :
		Object(std::move(other)),
		m_value(other.m_value)
	{
	}

	Float64& Float64::operator=(const Float64& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	Float64& Float64::operator=(Float64&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = other.m_value;
		}
		return *this;
	}

	void Float64::Set(cct::Float64 value)
	{
		if (m_value == value)
			return;
		m_value = value;
		if (!HasFlag(ObjectFlags::Constructing))
		{
			CCT_ASSERT_DOMAIN_THREAD();
			SetFlag(ObjectFlags::Dirty);
			OnValueChanged.Emit();
		}
	}

	cct::Float64 Float64::Get() const
	{
		return m_value;
	}
	Float64::operator cct::Float64() const
	{
		return m_value;
	}

	Float64& Float64::operator=(cct::Float64 value)
	{
		Set(value);
		return *this;
	}

	bool Float64::operator==(const Float64& other) const
	{
		return m_value == other.m_value;
	}
	bool Float64::operator!=(const Float64& other) const
	{
		return m_value != other.m_value;
	}
	bool Float64::operator==(cct::Float64 other) const
	{
		return m_value == other;
	}
	bool Float64::operator!=(cct::Float64 other) const
	{
		return m_value != other;
	}

	std::string Float64::ToString() const
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(15) << m_value;
		std::string s = oss.str();
		s.erase(s.find_last_not_of('0') + 1);
		if (s.back() == '.')
		{
			s.pop_back();
		}
		return s;
	}
} // namespace cct::refl
