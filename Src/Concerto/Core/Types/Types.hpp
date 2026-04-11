//
// Created by arthur on 22/02/2023.
//

#ifndef CONCERTO_CORE_TYPES_HPP
#define CONCERTO_CORE_TYPES_HPP

#include <cstdint>
#include "Concerto/Core/Defines.hpp"

namespace cct
{
	using Int8 = std::int8_t;
	using Int16 = std::int16_t;
	using Int32 = std::int32_t;
	using Int64 = std::int64_t;

	using UInt8 = std::uint8_t;
	using UInt16 = std::uint16_t;
	using UInt32 = std::uint32_t;
	using UInt64 = std::uint64_t;

	using Float32 = float;
	using Float64 = double;

	using Byte = std::int8_t;
	using UByte = std::uint8_t;

	CCT_CORE_PUBLIC_API bool IsDebuggerAttached();
}; // namespace cct


#endif // CONCERTO_CORE_TYPES_HPP
