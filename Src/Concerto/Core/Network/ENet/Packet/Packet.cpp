//
// Created by arthur on 14/08/2023.
//

#ifdef CCT_ENABLE_ENET

#include <cstring>

#include "Concerto/Core/Network/ENet/Packet/Packet.hpp"

namespace cct::net
{
	ENetPacket::ENetPacket() : Stream()
	{
	}

	ENetPacket::ENetPacket(const void* data, std::size_t size) : Stream(size)
	{
		std::memcpy(_buffer.data(), data, size);
		_cursorPos = size;
	}

	const Byte* ENetPacket::GetData() const
	{
		return _buffer.data();
	}

	std::size_t ENetPacket::Capacity() const
	{
		return _buffer.capacity();
	}

	bool ENetPacket::operator==(const ENetPacket& value) const
	{
		if (GetSize() != value.GetSize())
			return false;
		return std::memcmp(GetData(), value.GetData(), GetSize()) == 0;
	}

	bool ENetPacket::operator != (const ENetPacket& value) const
	{
		return !operator==(value);
	}
}

#endif // CCT_ENABLE_ENET