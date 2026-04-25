//
// Created by arthur on 31/12/2025
//

#ifndef CONCERTO_REFLECTION_ENUM_ITERATOR_HPP
#define CONCERTO_REFLECTION_ENUM_ITERATOR_HPP

#include <memory>
#include <span>
#include <string_view>

#include <Concerto/Core/Types/Types.hpp>

namespace cct::refl
{
	class EnumValue;

	class EnumIterator
	{
	public:
		explicit EnumIterator(std::span<const std::unique_ptr<EnumValue>> enumValues, std::size_t index = 0) :
			m_enumValues(enumValues),
			m_index(index)
		{
		}

		inline std::string_view GetName() const;
		inline cct::Int64 GetValue() const;
		inline std::size_t GetIndex() const;

		inline EnumIterator& operator++();
		inline bool operator!=(const EnumIterator& other) const;
		inline EnumIterator operator*() const
		{
			return *this;
		}

	private:
		std::span<const std::unique_ptr<EnumValue>> m_enumValues;
		std::size_t m_index;

		friend class EnumerationClass;
		friend class Enumeration;
	};

	class EnumIterable
	{
	public:
		explicit EnumIterable(std::span<const std::unique_ptr<EnumValue>> enumValues) :
			m_enumValues(enumValues)
		{
		}

		EnumIterator begin() const
		{
			return EnumIterator(m_enumValues, 0);
		}

		EnumIterator end() const
		{
			return EnumIterator(m_enumValues, m_enumValues.size());
		}

	private:
		std::span<const std::unique_ptr<EnumValue>> m_enumValues;
	};
} // namespace cct::refl

namespace std
{
	template<>
	struct tuple_size<cct::refl::EnumIterator> : std::integral_constant<std::size_t, 2>
	{
	};

	template<>
	struct tuple_element<0, cct::refl::EnumIterator>
	{
		using type = std::string_view;
	};

	template<>
	struct tuple_element<1, cct::refl::EnumIterator>
	{
		using type = cct::Int64;
	};
} // namespace std

namespace cct::refl
{
	template<std::size_t N>
	auto get(const EnumIterator& it) noexcept
	{
		static_assert(N < 2, "EnumIterator has only 2 elements: name and value");
		if constexpr (N == 0)
			return it.GetName();
		else if constexpr (N == 1)
			return it.GetValue();
	}
} // namespace cct::refl

#include "Concerto/Reflection/Enumeration/EnumIterator.inl"

#endif // CONCERTO_REFLECTION_ENUM_ITERATOR_HPP
