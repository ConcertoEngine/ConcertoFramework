//
// Created by arthur on 02/06/2024.
//

#ifndef CONCERTO_CORE_ERROR_HPP
#define CONCERTO_CORE_ERROR_HPP
#include <string>

#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	class CCT_CORE_PUBLIC_API Error
	{
	public:
		static std::string GetLastSystemErrorString();
	};
}
#endif // CONCERTO_CORE_ERROR_HPP
