//
// Created by arthur on 08/08/2024.
//

#ifndef CONCERTO_CORE_CAST_INL
#define CONCERTO_CORE_CAST_INL

#include <utility>

namespace cct
{
	template<typename To, typename From>
		requires(std::is_base_of_v<std::remove_cvref_t<From>, std::remove_cvref_t<To>> ||
					std::is_base_of_v<std::remove_cvref_t<To>, std::remove_cvref_t<From>>)
	inline To Cast(From&& value) noexcept
	{
		return static_cast<To>(std::forward<From>(value));
	}
}

#endif //CONCERTO_CORE_CAST_INL