#include "Concerto/Reflection/Folder/Folder.refl.hpp"

namespace cct::refl
{
	Folder::Folder(std::string_view value) :
		m_value(value)
	{
	}

	Folder::Folder(std::string value) :
		m_value(std::move(value))
	{
	}

	Folder::Folder(const Folder& other) :
		Object(other),
		m_value(other.m_value)
	{
	}

	Folder::Folder(Folder&& other) noexcept :
		Object(std::move(other)),
		m_value(std::move(other.m_value))
	{
	}

	Folder& Folder::operator=(const Folder& other)
	{
		if (this != &other)
		{
			Object::operator=(other);
			m_value = other.m_value;
		}
		return *this;
	}

	Folder& Folder::operator=(Folder&& other) noexcept
	{
		if (this != &other)
		{
			Object::operator=(std::move(other));
			m_value = std::move(other.m_value);
		}
		return *this;
	}

	void Folder::Set(std::string_view value)
	{
		m_value = value;
		OnValueChanged.Emit();
	}

	void Folder::Set(std::string value)
	{
		m_value = std::move(value);
		OnValueChanged.Emit();
	}

	const std::string& Folder::Get() const
	{
		return m_value;
	}

	std::string& Folder::Get()
	{
		return m_value;
	}

	Folder::operator std::string_view() const
	{
		return m_value;
	}

	Folder& Folder::operator=(std::string_view value)
	{
		Set(value);
		return *this;
	}

	Folder& Folder::operator=(std::string value)
	{
		Set(std::move(value));
		return *this;
	}

	bool Folder::operator==(const Folder& other) const
	{
		return m_value == other.m_value;
	}

	bool Folder::operator!=(const Folder& other) const
	{
		return m_value != other.m_value;
	}

	bool Folder::operator==(std::string_view other) const
	{
		return m_value == other;
	}

	bool Folder::operator!=(std::string_view other) const
	{
		return m_value != other;
	}

	std::string Folder::ToString() const
	{
		return m_value;
	}
} // namespace cct::refl
