#ifndef CONCERTO_CORE_UUID_HPP
#define CONCERTO_CORE_UUID_HPP

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

#include "Concerto/Core/Defines.hpp"
#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	class CCT_CORE_PUBLIC_API Uuid
	{
	public:
		using Bytes = std::array<UInt8, 16>;

		constexpr Uuid() = default;
		explicit constexpr Uuid(const Bytes& bytes) :
			m_bytes(bytes)
		{
		}

		[[nodiscard]] static Uuid Generate();
		[[nodiscard]] static Uuid FromString(std::string_view str);

		[[nodiscard]] std::string ToString() const;
		[[nodiscard]] bool IsNil() const
		{
			return m_bytes == Bytes{};
		}
		[[nodiscard]] const Bytes& GetBytes() const
		{
			return m_bytes;
		}

		[[nodiscard]] bool operator==(const Uuid& other) const = default;
		[[nodiscard]] bool operator!=(const Uuid& other) const = default;

	private:
		Bytes m_bytes{};
	};
} // namespace cct

#endif // CONCERTO_CORE_UUID_HPP
