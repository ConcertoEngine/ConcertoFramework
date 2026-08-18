//
// Created by arthur on 27/02/2026.
//

#include <Concerto/Core/Assert.hpp>
#include <Concerto/Core/ThreadAffinity/ThreadAffinity.hpp>

#include "Concerto/Reflection/Class/Class.hpp"
#include "Concerto/Reflection/FieldVisitor/FieldVisitor.hpp"
#include "Concerto/Reflection/Registry/Registry.hpp"
#include "Concerto/Reflection/Vector/Vector.refl.hpp"

namespace cct::refl
{
	Vector::Vector() :
		m_elementType(nullptr)
	{
	}

	Vector::~Vector()
	{
		ReleaseAll();
	}

	Vector::Vector(Vector&& other) noexcept :
		Object(std::move(other)),
		m_elementType(other.m_elementType),
		m_handles(std::move(other.m_handles)),
		m_elementRegistry(other.m_elementRegistry),
		m_ownRegistry(std::move(other.m_ownRegistry))
	{
		other.m_handles.clear();
		other.m_elementRegistry = nullptr;
	}

	Vector& Vector::operator=(Vector&& other) noexcept
	{
		if (this == &other)
			return *this;

		ReleaseAll();

		Object::operator=(std::move(other));
		m_elementType = other.m_elementType;
		m_handles = std::move(other.m_handles);
		m_elementRegistry = other.m_elementRegistry;
		m_ownRegistry = std::move(other.m_ownRegistry);

		other.m_handles.clear();
		other.m_elementRegistry = nullptr;
		return *this;
	}

	Registry& Vector::ElementRegistry(const Object* incoming)
	{
		if (m_elementRegistry != nullptr)
			return *m_elementRegistry;

		if (incoming != nullptr && incoming->GetRegistry() != nullptr)
		{
			m_elementRegistry = const_cast<Registry*>(incoming->GetRegistry());
			return *m_elementRegistry;
		}

		if (Registry* own = GetRegistry())
		{
			m_elementRegistry = own;
			return *m_elementRegistry;
		}

		m_ownRegistry = std::make_unique<Registry>();
		m_elementRegistry = m_ownRegistry.get();
		return *m_elementRegistry;
	}

	Registry* Vector::GetElementRegistry() const
	{
		return m_elementRegistry;
	}

	void Vector::ReleaseAll()
	{
		if (m_elementRegistry == nullptr)
		{
			m_handles.clear();
			return;
		}

		for (Handle handle : m_handles)
		{
			std::unique_ptr<Object> owned = m_elementRegistry->Release(handle);
		}
		m_handles.clear();

		if (m_elementRegistry != m_ownRegistry.get())
			m_elementRegistry = nullptr;
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

		Registry& registry = ElementRegistry(element.get());
		const std::size_t index = m_handles.size();
		const Handle handle = registry.Adopt(std::move(element));
		if (handle.IsNull())
		{
			CCT_ASSERT_FALSE("Vector::Add failed to adopt element");
			return;
		}
		m_handles.push_back(handle);
		CCT_ASSERT_DOMAIN_THREAD();
		SetFlag(ObjectFlags::Dirty);

		if (!HasFlag(ObjectFlags::Constructing))
		{
			Object* stored = registry.Resolve(handle);
			if (stored != nullptr)
			{
				OnInserted.Emit(index, *stored);
				OnValueChanged.Emit();
			}
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

		Registry& registry = ElementRegistry(element.get());
		const std::size_t index = m_handles.size();
		const Handle handle = registry.Adopt(std::move(element));
		if (handle.IsNull())
			return nullptr;
		m_handles.push_back(handle);
		CCT_ASSERT_DOMAIN_THREAD();
		SetFlag(ObjectFlags::Dirty);

		Object* ptr = registry.Resolve(handle);
		if (ptr != nullptr && !HasFlag(ObjectFlags::Constructing))
		{
			OnInserted.Emit(index, *ptr);
			OnValueChanged.Emit();
		}

		return ptr;
	}

	void Vector::Remove(std::size_t index)
	{
		if (index >= m_handles.size())
		{
			CCT_ASSERT_FALSE("Vector::Remove index {} out of range (size = {})", index, m_handles.size());
			return;
		}

		if (!HasFlag(ObjectFlags::Constructing))
		{
			// Emit before removal so listeners can still access the element
			if (Object* elem = Get(index))
			{
				OnRemoved.Emit(index, *elem);
			}
		}

		std::unique_ptr<Object> owned = m_elementRegistry->Release(m_handles[index]);
		m_handles.erase(m_handles.begin() + static_cast<std::ptrdiff_t>(index));
		CCT_ASSERT_DOMAIN_THREAD();
		SetFlag(ObjectFlags::Dirty);

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnValueChanged.Emit();
		}
	}

	void Vector::Clear()
	{
		if (m_handles.empty())
			return;

		// OnCleared always emits (cleanup handlers must run even during construction)
		OnCleared.Emit();

		ReleaseAll();
		CCT_ASSERT_DOMAIN_THREAD();
		SetFlag(ObjectFlags::Dirty);

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnValueChanged.Emit();
		}
	}

	void Vector::Move(std::size_t from, std::size_t to)
	{
		const std::size_t count = m_handles.size();
		if (from == to || from >= count || to >= count)
			return;

		const Handle handle = m_handles[from];
		m_handles.erase(m_handles.begin() + static_cast<std::ptrdiff_t>(from));
		m_handles.insert(m_handles.begin() + static_cast<std::ptrdiff_t>(to), handle);
		CCT_ASSERT_DOMAIN_THREAD();
		SetFlag(ObjectFlags::Dirty);

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnValueChanged.Emit();
		}
	}

	std::unique_ptr<Object> Vector::Extract(std::size_t index)
	{
		if (index >= m_handles.size())
			return nullptr;

		if (!HasFlag(ObjectFlags::Constructing))
		{
			if (Object* elem = Get(index))
			{
				OnRemoved.Emit(index, *elem);
			}
		}

		std::unique_ptr<Object> owned = m_elementRegistry->Release(m_handles[index]);
		m_handles.erase(m_handles.begin() + static_cast<std::ptrdiff_t>(index));
		CCT_ASSERT_DOMAIN_THREAD();
		SetFlag(ObjectFlags::Dirty);

		if (!HasFlag(ObjectFlags::Constructing))
		{
			OnValueChanged.Emit();
		}

		return owned;
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

		if (index > m_handles.size())
			index = m_handles.size();

		Registry& registry = ElementRegistry(element.get());
		const Handle handle = registry.Adopt(std::move(element));
		if (handle.IsNull())
			return;
		m_handles.insert(m_handles.begin() + static_cast<std::ptrdiff_t>(index), handle);
		CCT_ASSERT_DOMAIN_THREAD();
		SetFlag(ObjectFlags::Dirty);

		if (!HasFlag(ObjectFlags::Constructing))
		{
			Object* stored = registry.Resolve(handle);
			if (stored != nullptr)
			{
				OnInserted.Emit(index, *stored);
				OnValueChanged.Emit();
			}
		}
	}

	Object* Vector::Get(std::size_t index)
	{
		if (index >= m_handles.size())
		{
			CCT_ASSERT_FALSE("Vector::Get index {} out of range (size = {})", index, m_handles.size());
			return nullptr;
		}
		if (m_elementRegistry == nullptr)
			return nullptr;
		return m_elementRegistry->Resolve(m_handles[index]);
	}

	const Object* Vector::Get(std::size_t index) const
	{
		if (index >= m_handles.size())
		{
			CCT_ASSERT_FALSE("Vector::Get index {} out of range (size = {})", index, m_handles.size());
			return nullptr;
		}
		if (m_elementRegistry == nullptr)
			return nullptr;
		return m_elementRegistry->Resolve(m_handles[index]);
	}

	std::size_t Vector::GetCount() const
	{
		return m_handles.size();
	}

	bool Vector::IsEmpty() const
	{
		return m_handles.empty();
	}

	const Class* Vector::GetElementType() const
	{
		return m_elementType;
	}

	void Vector::Accept(FieldVisitor& visitor)
	{
		for (Handle handle : m_handles)
		{
			if (m_elementRegistry == nullptr)
				break;
			if (Object* elem = m_elementRegistry->Resolve(handle))
			{
				elem->Accept(visitor);
			}
		}
	}
} // namespace cct::refl
