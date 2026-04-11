//
// Created by Arthur on 06/10/2025.
//

#ifndef CONCERTO_CORE_ENUMFLAGS_HPP
#define CONCERTO_CORE_ENUMFLAGS_HPP

#include <type_traits>
#include "Concerto/Core/Defines.hpp"

namespace cct
{
	template<typename E>
	struct EnableEnumFlags : std::false_type {};

	template<typename E>
	requires(std::is_enum_v<E>)
	class EnumFlags
	{
	public:
		using Underlying = std::underlying_type_t<E>;

		constexpr EnumFlags() noexcept = default;
		constexpr EnumFlags(E e) noexcept : _value(static_cast<Underlying>(e)) {}
		constexpr explicit EnumFlags(Underlying raw) noexcept : _value(raw) {}

		[[nodiscard]] static constexpr EnumFlags FromRaw(Underlying raw) noexcept { return EnumFlags(raw); }

		[[nodiscard]] constexpr Underlying Value() const noexcept { return _value; }
		[[nodiscard]] constexpr bool Any() const noexcept { return _value != 0; }
		[[nodiscard]] constexpr bool None() const noexcept { return _value == 0; }

		[[nodiscard]] constexpr bool Contains(E e) const noexcept
		{
			auto mask = static_cast<Underlying>(e);
			return (static_cast<Underlying>(_value) & mask) == mask;
		}

		[[nodiscard]] constexpr bool Contains(EnumFlags f) const noexcept
		{
			return (static_cast<Underlying>(_value) & f._value) == f._value;
		}

		constexpr EnumFlags& Set(E e) noexcept
		{
			_value |= static_cast<Underlying>(e);
			return *this;
		}

		constexpr EnumFlags& Reset(E e) noexcept
		{
			_value &= ~static_cast<Underlying>(e);
			return *this;
		}

		constexpr EnumFlags& Toggle(E e) noexcept
		{
			_value ^= static_cast<Underlying>(e);
			return *this;
		}

		constexpr void Clear() noexcept { _value = 0; }

		[[nodiscard]] constexpr EnumFlags operator~() const noexcept
		{
			return EnumFlags(static_cast<Underlying>(~_value));
		}

		constexpr EnumFlags& operator|=(EnumFlags other) noexcept
		{
			_value |= other._value;
			return *this;
		}
		constexpr EnumFlags& operator|=(E e) noexcept
		{
			_value |= static_cast<Underlying>(e);
			return *this;
		}
		constexpr EnumFlags& operator&=(EnumFlags other) noexcept
		{
			_value &= other._value;
			return *this;
		}
		constexpr EnumFlags& operator&=(E e) noexcept
		{
			_value &= static_cast<Underlying>(e);
			return *this;
		}
		constexpr EnumFlags& operator^=(EnumFlags other) noexcept
		{
			_value ^= other._value;
			return *this;
		}
		constexpr EnumFlags& operator^=(E e) noexcept
		{
			_value ^= static_cast<Underlying>(e);
			return *this;
		}

		// Comparisons
		[[nodiscard]] constexpr bool operator==(EnumFlags other) const noexcept { return _value == other._value; }
		[[nodiscard]] constexpr bool operator!=(EnumFlags other) const noexcept { return _value != other._value; }

		// Truthiness: true if any bit set
		[[nodiscard]] constexpr explicit operator bool() const noexcept { return Any(); }

	private:
		Underlying _value = 0;
	};

	template<typename E>
	[[nodiscard]] constexpr EnumFlags<E> operator|(EnumFlags<E> lhs, EnumFlags<E> rhs) noexcept
	{
		return EnumFlags<E>::FromRaw(static_cast<typename EnumFlags<E>::Underlying>(lhs.Value() | rhs.Value()));
	}
	template<typename E>
	[[nodiscard]] constexpr EnumFlags<E> operator&(EnumFlags<E> lhs, EnumFlags<E> rhs) noexcept
	{
		return EnumFlags<E>::FromRaw(static_cast<typename EnumFlags<E>::Underlying>(lhs.Value() & rhs.Value()));
	}
	template<typename E>
	[[nodiscard]] constexpr EnumFlags<E> operator^(EnumFlags<E> lhs, EnumFlags<E> rhs) noexcept
	{
		return EnumFlags<E>::FromRaw(static_cast<typename EnumFlags<E>::Underlying>(lhs.Value() ^ rhs.Value()));
	}

	// Mixed operators with enum values
	template<typename E>
	[[nodiscard]] constexpr EnumFlags<E> operator|(EnumFlags<E> lhs, E rhs) noexcept
	{
		lhs |= rhs;
		return lhs;
	}
	template<typename E>
	[[nodiscard]] constexpr EnumFlags<E> operator|(E lhs, EnumFlags<E> rhs) noexcept
	{
		rhs |= lhs;
		return rhs;
	}
	template<typename E>
	[[nodiscard]] constexpr EnumFlags<E> operator&(EnumFlags<E> lhs, E rhs) noexcept
	{
		lhs &= rhs;
		return lhs;
	}
	template<typename E>
	[[nodiscard]] constexpr EnumFlags<E> operator&(E lhs, EnumFlags<E> rhs) noexcept
	{
		rhs &= lhs;
		return rhs;
	}
	template<typename E>
	[[nodiscard]] constexpr EnumFlags<E> operator^(EnumFlags<E> lhs, E rhs) noexcept
	{
		lhs ^= rhs;
		return lhs;
	}
	template<typename E>
	[[nodiscard]] constexpr EnumFlags<E> operator^(E lhs, EnumFlags<E> rhs) noexcept
	{
		rhs ^= lhs;
		return rhs;
	}

	template<typename E>
		requires(std::is_enum_v<E>&& EnableEnumFlags<E>::value)
	[[nodiscard]] constexpr EnumFlags<E> operator|(E lhs, E rhs) noexcept
	{
		return EnumFlags<E>(lhs) | rhs;
	}
	template<typename E>
		requires(std::is_enum_v<E>&& EnableEnumFlags<E>::value)
	[[nodiscard]] constexpr EnumFlags<E> operator&(E lhs, E rhs) noexcept
	{
		return EnumFlags<E>(lhs) & rhs;
	}
	template<typename E>
		requires(std::is_enum_v<E>&& EnableEnumFlags<E>::value)
	[[nodiscard]] constexpr EnumFlags<E> operator^(E lhs, E rhs) noexcept
	{
		return EnumFlags<E>(lhs) ^ rhs;
	}
	template<typename E>
		requires(std::is_enum_v<E>&& EnableEnumFlags<E>::value)
	[[nodiscard]] constexpr EnumFlags<E> operator~(E e) noexcept
	{
		return ~EnumFlags<E>(e);
	}
}

#define CCT_ENABLE_ENUM_FLAGS(EnumType) namespace cct { template<> struct EnableEnumFlags<EnumType> : std::true_type {};}

#endif // CONCERTO_CORE_ENUMFLAGS_HPP

