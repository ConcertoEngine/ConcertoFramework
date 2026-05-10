#include "Concerto/Reflection/File/File.refl.hpp"

namespace cct::refl
{
	File::File(std::string_view value) :
		m_value(value)
	{
	}

	File::File(std::string value) :
		m_value(std::move(value))
	{
	}

	File::File(const File& other) :
		Object(other),
		m_value(other.m_value)
	{
	}

	File::File(File&& other) noexcept :
		Object(std::move(other)),
		m_value(std::move(other.m_value))
	{
	}

	File& File::operator=(const File& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	File& File::operator=(File&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = std::move(other.m_value);
		}
		return *this;
	}

	void File::Set(std::string_view value)
	{
		m_value = value;
		OnValueChanged.Emit();
	}

	void File::Set(std::string value)
	{
		m_value = std::move(value);
		OnValueChanged.Emit();
	}

	const std::string& File::Get() const
	{
		return m_value;
	}

	std::string& File::Get()
	{
		return m_value;
	}

	File::operator std::string_view() const
	{
		return m_value;
	}

	File& File::operator=(std::string_view value)
	{
		Set(value);
		return *this;
	}

	File& File::operator=(std::string value)
	{
		Set(std::move(value));
		return *this;
	}

	bool File::operator==(const File& other) const
	{
		return m_value == other.m_value;
	}

	bool File::operator!=(const File& other) const
	{
		return m_value != other.m_value;
	}

	bool File::operator==(std::string_view other) const
	{
		return m_value == other;
	}

	bool File::operator!=(std::string_view other) const
	{
		return m_value != other;
	}

	std::string File::ToString() const
	{
		return m_value;
	}
} // namespace cct::refl
