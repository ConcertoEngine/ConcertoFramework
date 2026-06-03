//
// Created by arthur on 29/05/2023.
//

#ifndef CONCERTO_CORE_STREAM_INL
#define CONCERTO_CORE_STREAM_INL

#include "Concerto/Core/Serializer/Serializer.hpp"
#include "Concerto/Core/Stream/Stream.hpp"

namespace cct
{
	template<typename T>
	Stream& Stream::operator<<(const T& data)
	{
		cct::Serialize(*this, data);
		return *this;
	}

	template<typename T>
	Stream& Stream::operator>>(T& data)
	{
		cct::Deserialize(*this, data);
		return *this;
	}
} // namespace cct

#endif // CONCERTO_CORE_STREAM_INL
