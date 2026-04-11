//
// Created by Arthur on 08/10/2025.
//

#ifndef CONCERTO_CORE_ANY_INL
#define CONCERTO_CORE_ANY_INL

#include <new>
#include <stdexcept>
#include <utility>

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/TypeInfo/TypeInfo.hpp"
#include "Concerto/Core/Any/Any.hpp"

namespace cct
{
	inline Any::Any() noexcept :
		m_typeId(0),
		m_storage(AlignedBuffer{}),
		m_destroy(nullptr),
		m_copy(nullptr),
		m_move(nullptr)
	{
	}

	inline Any::~Any()
	{
		Reset();
	}

	inline Any::Any(const Any& other) :
		m_typeId(0),
		m_storage(AlignedBuffer{}),
		m_destroy(nullptr),
		m_copy(nullptr),
		m_move(nullptr)
	{
		if (other.m_typeId != 0 && other.m_copy)
		{
			other.m_copy(other, *this);
		}
	}

	inline Any::Any(Any&& other) noexcept :
		m_typeId(0),
		m_storage(AlignedBuffer{}),
		m_destroy(nullptr),
		m_copy(nullptr),
		m_move(nullptr)
	{
		if (other.m_typeId != 0)
		{
			other.m_move(other, *this);
		}
	}

	inline Any& Any::operator=(const Any& other)
	{
		if (this == &other)
			return *this;
		Reset();
		if (other.m_typeId != 0 && other.m_copy)
			other.m_copy(other, *this);
		return *this;
	}

	inline Any& Any::operator=(Any&& other) noexcept
	{
		if (this == &other)
			return *this;
		Reset();
		if (other.m_typeId != 0)
			other.m_move(other, *this);
		return *this;
	}

	template<typename T, typename... Args>
	Any& Any::operator=(Args&&... args)
	{
		Emplace<T>(std::forward<Args>(args)...);
		return *this;
	}

	template<typename T>
	Any& Any::operator=(T&& data)
	{
		using Exposed = T;
		Emplace<Exposed>(std::forward<T>(data));
		return *this;
	}

	inline void Any::Reset()
	{
		if (m_typeId != 0 && m_destroy)
			m_destroy(*this);
		m_typeId = 0;
		m_destroy = nullptr;
		m_copy = nullptr;
		m_move = nullptr;
		m_storage = AlignedBuffer{};
	}

	inline bool Any::HasValue() const noexcept
	{
		return m_typeId != 0;
	}

	template<typename T>
	bool Any::Is() const noexcept
	{
		return m_typeId == TypeId<T>();
	}

	template<typename T>
	T Any::As()
	{
		using Exposed = T;
		using Stored = typename Traits<Exposed>::Stored;

		if (!Is<Exposed>())
		{
			CCT_ASSERT_FALSE("Any::As: invalid cast");
			throw std::bad_cast();
		}

		void* raw = DataPtr();
		Stored* s = static_cast<Stored*>(raw);

		if constexpr (std::is_reference_v<Exposed>)
			return *(*s); // s is U*, return U&
		else if constexpr (std::is_pointer_v<Exposed>)
			return *s; // stored is pointer value
		else
			return static_cast<Exposed>(*s);
	}

	template<typename T, typename... Args>
	Any Any::Make(Args&&... args)
	{
		Any a;
		a.Emplace<T>(std::forward<Args>(args)...);
		return a;
	}

	template<typename T, typename... Args>
	void Any::Emplace(Args&&... args)
	{
		EmplaceImpl<T>(std::forward<Args>(args)...);
	}

	inline void* Any::StackPtr() noexcept
	{
		return &std::get<AlignedBuffer>(m_storage).data[0];
	}

	inline const void* Any::StackPtr() const noexcept
	{
		return &std::get<AlignedBuffer>(m_storage).data[0];
	}

	inline void* Any::DataPtr() noexcept
	{
		if (std::holds_alternative<void*>(m_storage))
			return std::get<void*>(m_storage);
		return StackPtr();
	}

	inline const void* Any::DataPtr() const noexcept
	{
		if (std::holds_alternative<void*>(m_storage))
			return std::get<void*>(m_storage);
		return StackPtr();
	}

	template<typename Exposed, typename... Args>
	void Any::EmplaceImpl(Args&&... args)
	{
		using TraitsT = Traits<Exposed>;
		using Stored = typename TraitsT::Stored;

		Reset();

		m_typeId = TypeId<Exposed>();
		m_destroy = &DestroyImpl<Exposed>;
		m_copy = &CopyImpl<Exposed>;
		m_move = &MoveImpl<Exposed>;

		if constexpr (TraitsT::FitsInStack)
		{
			void* dst = StackPtr();
			if constexpr (std::is_reference_v<Exposed>)
			{
				static_assert(sizeof...(Args) == 1, "Emplace<Ref>: requires exactly one argument (the referenced object)");
				auto&& ref = (std::forward<Args>(args), ...);
				using U = std::remove_reference_t<Exposed>;
				new (dst) Stored(const_cast<U*>(std::addressof(ref)));
			}
			else
			{
				new (dst) Stored(std::forward<Args>(args)...);
			}
		}
		else
		{
			if constexpr (std::is_reference_v<Exposed>)
			{
				static_assert(sizeof...(Args) == 1, "Emplace<Ref>: requires exactly one argument (the referenced object)");
				auto&& ref = (std::forward<Args>(args), ...);
				using U = std::remove_reference_t<Exposed>;
				m_storage = static_cast<void*>(new Stored(const_cast<U*>(std::addressof(ref))));
			}
			else
			{
				m_storage = static_cast<void*>(new Stored(std::forward<Args>(args)...));
			}
		}
	}

	template<typename Exposed>
	void Any::DestroyImpl(Any& a)
	{
		using Stored = typename Traits<Exposed>::Stored;
		if (std::holds_alternative<void*>(a.m_storage))
		{
			auto* p = reinterpret_cast<Stored*>(std::get<void*>(a.m_storage));
			delete p;
			a.m_storage = AlignedBuffer{};
		}
		else
		{
			if constexpr (!std::is_pointer_v<Stored>)
			{
				auto* p = reinterpret_cast<Stored*>(a.StackPtr());
				p->~Stored();
			}
		}
	}

	template<typename Exposed>
	void Any::CopyImpl(const Any& src, Any& dst)
	{
		using Stored = typename Traits<Exposed>::Stored;
		if constexpr (Traits<Exposed>::FitsInStack)
		{
			if constexpr (std::is_reference_v<Exposed>)
			{
				auto* s = reinterpret_cast<const Stored*>(src.StackPtr());
				dst.EmplaceImpl<Exposed>(*(*s));
			}
			else
			{
				auto* s = reinterpret_cast<const Stored*>(src.StackPtr());
				dst.EmplaceImpl<Exposed>(*s);
			}
		}
		else
		{
			if constexpr (std::is_reference_v<Exposed>)
			{
				auto* s = reinterpret_cast<const Stored*>(std::get<void*>(src.m_storage));
				dst.EmplaceImpl<Exposed>(*(*s));
			}
			else
			{
				auto* s = reinterpret_cast<const Stored*>(std::get<void*>(src.m_storage));
				dst.EmplaceImpl<Exposed>(*s);
			}
		}
	}

	template<typename Exposed>
	void Any::MoveImpl(Any& src, Any& dst)
	{
		using Stored = typename Traits<Exposed>::Stored;

		// move metadata
		dst.m_typeId = src.m_typeId;
		dst.m_destroy = src.m_destroy;
		dst.m_copy = src.m_copy;
		dst.m_move = src.m_move;

		if (std::holds_alternative<void*>(src.m_storage))
		{
			dst.m_storage = std::get<void*>(src.m_storage);
			src.m_storage = AlignedBuffer{};
		}
		else
		{
			if constexpr (Traits<Exposed>::FitsInStack)
			{
				auto* s = static_cast<Stored*>(src.StackPtr());
				void* d = dst.StackPtr();
				new (d) Stored(std::move(*s));
				if constexpr (!std::is_pointer_v<Stored>)
					s->~Stored();
			}
			else
			{
				// should not happen since non-fitting types are heap-allocated
				dst.m_storage = nullptr;
			}
		}

		src.m_typeId = 0;
		src.m_destroy = nullptr;
		src.m_copy = nullptr;
		src.m_move = nullptr;
	}
}

#endif // CONCERTO_CORE_ANY_INL
