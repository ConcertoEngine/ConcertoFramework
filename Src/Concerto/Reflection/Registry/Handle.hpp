//
// Created by arthur on 18/08/2026.
//

#ifndef CONCERTO_REFLECTION_HANDLE_HPP
#define CONCERTO_REFLECTION_HANDLE_HPP

#include <cstddef>
#include <cstdint>
#include <functional>

namespace cct::refl
{
	struct Handle
	{
		std::uint32_t m_index = 0;
		std::uint32_t m_generation = 0;

		[[nodiscard]] bool IsNull() const noexcept
		{
			return m_generation == 0;
		}

		bool operator==(const Handle& other) const noexcept = default;
	};
} // namespace cct::refl

template<>
struct std::hash<cct::refl::Handle>
{
	[[nodiscard]] std::size_t operator()(const cct::refl::Handle& handle) const noexcept
	{
		return (static_cast<std::size_t>(handle.m_generation) << 32) ^ static_cast<std::size_t>(handle.m_index);
	}
};

#endif // CONCERTO_REFLECTION_HANDLE_HPP
