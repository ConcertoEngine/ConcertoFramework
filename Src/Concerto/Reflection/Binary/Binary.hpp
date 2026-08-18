//
// Created by arthur on 18/08/2026.
//

#ifndef CONCERTO_REFLECTION_BINARY_HPP
#define CONCERTO_REFLECTION_BINARY_HPP

#include <cstddef>
#include <span>
#include <vector>

#include "Concerto/Reflection/Defines.hpp"

namespace cct::refl
{
	class Object;
	class Registry;

	class CCT_REFLECTION_API Binary
	{
	public:
		static std::vector<std::byte> ToBinary(const Object& obj, bool emitClass = true);
		static bool FromBinary(Object& target, std::span<const std::byte> data, Registry* registry = nullptr);
	};
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_BINARY_HPP
