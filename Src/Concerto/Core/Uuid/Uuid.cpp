#include <cstring>

#define UUID_SYSTEM_GENERATOR
#include <stduuid/uuid.h>

#include "Concerto/Core/Uuid/Uuid.hpp"

namespace cct
{
	namespace
	{
		Uuid::Bytes UuidToBytes(const uuids::uuid& id)
		{
			Uuid::Bytes bytes;
			std::memcpy(bytes.data(), id.as_bytes().data(), 16);
			return bytes;
		}
	} // namespace

	Uuid Uuid::Generate()
	{
		return Uuid(UuidToBytes(uuids::uuid_system_generator{}()));
	}

	Uuid Uuid::FromString(std::string_view str)
	{
		const auto parsed = uuids::uuid::from_string(str);
		if (!parsed)
			return {};
		return Uuid(UuidToBytes(*parsed));
	}

	std::string Uuid::ToString() const
	{
		return uuids::to_string(uuids::uuid(m_bytes.begin(), m_bytes.end()));
	}
} // namespace cct
