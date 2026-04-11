//
// Created by arthur on 22/02/2023.
//

#ifndef CONCERTO_CORE_TYPE_HPP
#define CONCERTO_CORE_TYPE_HPP

#include <string_view>
#include <functional>

#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	template<typename T>
	constexpr std::string_view TypeName()
	{
		//from https://github.com/NazaraEngine/NazaraUtils/blob/main/include/NazaraUtils/TypeName.inl
		using namespace std::string_view_literals;

		constexpr std::string_view functionSignature = CCT_FUNCTION_SIGNATURE;
#ifdef CCT_COMPILER_MSVC
		constexpr auto prefix = "cct::TypeName<"sv;
		constexpr auto suffix = ">"sv;
#elif defined(CCT_COMPILER_GCC)
		constexpr auto prefix = "T = "sv;
		constexpr auto suffix = ";"sv;
#elif defined(CCT_COMPILER_CLANG)
		constexpr auto prefix = "T = "sv;
		constexpr auto suffix = ";]"sv;
#endif
		constexpr auto prefixStartPos = functionSignature.find(prefix);
		static_assert(prefixStartPos != std::string_view::npos);

		constexpr auto suffixPos = functionSignature.substr(prefixStartPos).find_last_of(suffix);
		static_assert(suffixPos != std::string_view::npos);

		constexpr auto prefixEndPos = prefixStartPos + prefix.size();

		constexpr auto typeName = functionSignature.substr(prefixEndPos, suffixPos - prefix.size());

#ifdef CCT_COMPILER_MSVC

		for (std::string_view p : {"class "sv, "struct "sv, "enum class "sv, "enum struct "sv, "enum "sv})
		{
			if (typeName.starts_with(p))
			{
				return typeName.substr(p.size());
			}
		}
#endif
		return typeName;
	}

	/**
	 * @brief Compute the hash of a type (using cyclic redundancy check)
	 * @tparam T Type to hash
	 * @return Hash of the type
	 */
	template<typename T>
	constexpr UInt64 TypeId()
	{
		constexpr std::string_view typeName = TypeName<T>();
		return std::hash<std::string_view>{}(typeName); //FIXME: Use constexpr CRC32/64
	}
}

#endif
