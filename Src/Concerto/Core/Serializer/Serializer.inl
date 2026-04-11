//
// Created by arthur on 29/05/2023.
//

#ifndef CONCERTO_CORE_SERIALIZER_INL
#define CONCERTO_CORE_SERIALIZER_INL

#include "Concerto/Core/Serializer/Serializer.hpp"
#include "Concerto/Core/Stream/Stream.hpp"
#include "Concerto/Core/ByteSwap.hpp"

namespace cct
{
	template<typename T>
	requires(std::is_same_v<T, bool>)
	void Serialize(Stream& stream, T value)
	{
		stream.Write(&value, sizeof(T));
	}

	template<typename T>
	requires ((std::is_integral_v<T> || std::is_floating_point_v<T>) && !std::is_same_v<T, bool>)
	void Serialize(Stream& stream, T value)
	{
		value = ByteSwap(value);
		stream.Write(&value, sizeof(T));
	}

	template<typename T>
	requires ((std::is_integral_v<T> || std::is_floating_point_v<T>) && !std::is_same_v<T, bool>)
	void Deserialize(Stream& stream, T& value)
	{
		stream.Read(&value, sizeof(T));
		value = ByteSwap(value);
	}

	template<typename T>
	requires(std::is_same_v<T, bool>)
	void Deserialize(Stream& stream, T& value)
	{
		stream.Read(&value, sizeof(T));
	}
}

#endif //CONCERTO_CORE_SERIALIZER_INL
