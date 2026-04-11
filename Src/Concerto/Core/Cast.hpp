//
// Created by arthur on 08/08/2024.
//

#ifndef CONCERTO_CORE_CAST_HPP
#define CONCERTO_CORE_CAST_HPP

#include <type_traits>

namespace cct
{
	template<typename To, typename From>
		requires(std::is_base_of_v<std::remove_cvref_t<From>, std::remove_cvref_t<To>> ||
					std::is_base_of_v<std::remove_cvref_t<To>, std::remove_cvref_t<From>>)
	inline To Cast(From&& value) noexcept;
}

#include "Concerto/Core/Cast/Cast.inl"

#endif //CONCERTO_CORE_CAST_HPP