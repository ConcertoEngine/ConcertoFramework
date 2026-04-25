//
// Created by arthur on 27/02/2026.
//

#include "Concerto/Reflection/Vector/Vector.refl.hpp"

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class/Class.hpp"

namespace cct::refl
{
	Vector::Vector() :
		m_elementType(nullptr)
	{
	}

	void Vector::Add(std::unique_ptr<Object> element)
	{
		if (!element)
		{
			CCT_ASSERT_FALSE("Vector::Add called with null element");
			return;
		}

		if (m_elementType && element->GetDynamicClass() != m_elementType)
		{
			CCT_ASSERT_FALSE("Vector::Add type mismatch: expected '{}', got '{}'",
							 m_elementType->GetName(),
							 element->GetDynamicClass() ? element->GetDynamicClass()->GetName() : "<null>");
			return;
		}

		const std::size_t index = m_elements.size();
		m_elements.push_back(std::move(element));

		OnInserted.Emit(index, *m_elements.back());
		OnValueChanged.Emit();
	}

	Object* Vector::Add()
	{
		if (!m_elementType)
		{
			CCT_ASSERT_FALSE("Vector::Add() called but m_elementType is not set");
			return nullptr;
		}

		auto element = m_elementType->CreateDefaultObject();
		if (!element)
		{
			CCT_ASSERT_FALSE("Vector::Add() failed to create default object for type '{}'",
							 m_elementType->GetName());
			return nullptr;
		}

		const std::size_t index = m_elements.size();
		Object* ptr = m_elements.emplace_back(std::move(element)).get();

		OnInserted.Emit(index, *ptr);
		OnValueChanged.Emit();

		return ptr;
	}

	void Vector::Remove(std::size_t index)
	{
		if (index >= m_elements.size())
		{
			CCT_ASSERT_FALSE("Vector::Remove index {} out of range (size = {})", index, m_elements.size());
			return;
		}

		// Emit before removal so listeners can still access the element
		OnRemoved.Emit(index, *m_elements[index]);

		m_elements.erase(m_elements.begin() + static_cast<std::ptrdiff_t>(index));
		OnValueChanged.Emit();
	}

	void Vector::Clear()
	{
		if (m_elements.empty())
			return;

		// Emit before clearing so listeners can react while elements are still alive
		OnCleared.Emit();

		m_elements.clear();
		OnValueChanged.Emit();
	}

	Object* Vector::Get(std::size_t index)
	{
		if (index >= m_elements.size())
		{
			CCT_ASSERT_FALSE("Vector::Get index {} out of range (size = {})", index, m_elements.size());
			return nullptr;
		}
		return m_elements[index].get();
	}

	const Object* Vector::Get(std::size_t index) const
	{
		if (index >= m_elements.size())
		{
			CCT_ASSERT_FALSE("Vector::Get index {} out of range (size = {})", index, m_elements.size());
			return nullptr;
		}
		return m_elements[index].get();
	}

	std::size_t Vector::GetCount() const
	{
		return m_elements.size();
	}

	bool Vector::IsEmpty() const
	{
		return m_elements.empty();
	}

	const Class* Vector::GetElementType() const
	{
		return m_elementType;
	}
} // namespace cct::refl
