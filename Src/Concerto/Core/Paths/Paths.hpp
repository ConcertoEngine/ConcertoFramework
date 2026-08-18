//
// Created by arthur on 14/08/2026.
//

#ifndef CONCERTO_CORE_PATHS_HPP
#define CONCERTO_CORE_PATHS_HPP

#include <filesystem>

#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	enum class StandardDirectory : UInt8
	{
		Home,
		Temp,
		Documents,
		Downloads,
		Videos,
		Pictures,
		Music,
		Desktop,
	};

	class CCT_CORE_PUBLIC_API Paths
	{
	public:
		static std::filesystem::path Get(StandardDirectory dir);
	};
} // namespace cct

#endif // CONCERTO_CORE_PATHS_HPP
