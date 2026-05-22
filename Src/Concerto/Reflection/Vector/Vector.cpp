//
// Created by arthur on 27/02/2026.
//

#include <Concerto/Core/Assert.hpp>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/FieldVisitor/FieldVisitor.hpp"
#include "Concerto/Reflection/Vector/Vector.refl.hpp"

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

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnInserted.Emit(index, *m_elements.back());
			OnValueChanged.Emit();
		}
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

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnInserted.Emit(index, *ptr);
			OnValueChanged.Emit();
		}

		return ptr;
	}

	void Vector::Remove(std::size_t index)
	{
		if (index >= m_elements.size())
		{
			CCT_ASSERT_FALSE("Vector::Remove index {} out of range (size = {})", index, m_elements.size());
			return;
		}

		if (!HasFlag(ObjectFlags::Constructing))
		{
			// Emit before removal so listeners can still access the element
			OnRemoved.Emit(index, *m_elements[index]);
		}

		m_elements.erase(m_elements.begin() + static_cast<std::ptrdiff_t>(index));

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnValueChanged.Emit();
		}
	}

	void Vector::Clear()
	{
		if (m_elements.empty())
			return;

		// OnCleared always emits (cleanup handlers must run even during construction)
		OnCleared.Emit();

		m_elements.clear();

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnValueChanged.Emit();
		}
	}

	void Vector::Move(std::size_t from, std::size_t to)
	{
		const std::size_t count = m_elements.size();
		if (from == to || from >= count || to >= count)
			return;

		auto elem = std::move(m_elements[from]);
		m_elements.erase(m_elements.begin() + static_cast<std::ptrdiff_t>(from));
		m_elements.insert(m_elements.begin() + static_cast<std::ptrdiff_t>(to), std::move(elem));

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnValueChanged.Emit();
		}
	}

	std::unique_ptr<Object> Vector::Extract(std::size_t index)
	{
		if (index >= m_elements.size())
			return nullptr;

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnRemoved.Emit(index, *m_elements[index]);
		}

		auto elem = std::move(m_elements[index]);
		m_elements.erase(m_elements.begin() + static_cast<std::ptrdiff_t>(index));

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnValueChanged.Emit();
		}

		return elem;
	}

	void Vector::Insert(std::size_t index, std::unique_ptr<Object> element)
	{
		if (!element)
			return;

		if (m_elementType && element->GetDynamicClass() != m_elementType)
		{
			CCT_ASSERT_FALSE("Vector::Insert type mismatch: expected '{}', got '{}'",
							 m_elementType->GetName(),
							 element->GetDynamicClass() ? element->GetDynamicClass()->GetName() : "<null>");
			return;
		}

		if (index > m_elements.size())
			index = m_elements.size();

		m_elements.insert(m_elements.begin() + static_cast<std::ptrdiff_t>(index), std::move(element));

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnInserted.Emit(index, *m_elements[index]);
			OnValueChanged.Emit();
		}
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

	void Vector::Accept(FieldVisitor& visitor)
	{
		for (const auto& elem : m_elements)
		{
			if (elem != nullptr)
			{
				elem->Accept(visitor);
			}
		}
	}
} // namespace cct::refl
