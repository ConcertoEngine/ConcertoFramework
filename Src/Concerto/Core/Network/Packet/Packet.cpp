//
// Created by arthur on 28/05/2023.
//

#include <cstring>

#include "Concerto/Core/Network/Packet/Packet.hpp"

namespace cct::net
{
	Packet::Packet() : Stream(HeaderSize), _size(0), _packetType(0), _validHeader(false)
	{
	}

	Packet::Packet(UInt8 packetType, const void* data, UInt32 size) :
		Stream(HeaderSize + size), _size(size), _packetType(packetType), _validHeader(true)
	{
		_buffer.resize(HeaderSize + size);
		std::memcpy(_buffer.data(), &_packetType, sizeof(UInt8));
		std::memcpy(_buffer.data() + sizeof(UInt8), &size, sizeof(UInt32));
		std::memcpy(_buffer.data() + HeaderSize, data, size);
		_cursorPos = HeaderSize + size;
	}

	Packet::Packet(UInt8 packetType, std::size_t capacity) :
		Stream(HeaderSize + capacity), _size(0), _packetType(packetType), _validHeader(true)
	{
		_buffer.resize(HeaderSize);
		std::memcpy(_buffer.data(), &_packetType, sizeof(UInt8));
		std::memcpy(_buffer.data() + sizeof(UInt8), &_size, sizeof(UInt32));
		_cursorPos = HeaderSize;
	}

	UInt8 Packet::GetPacketType() const
	{
		return _packetType;
	}

	const Byte* Packet::GetData() const
	{
		return _buffer.data();
	}

	UInt64 Packet::GetDataSize() const
	{
		return GetSize() - HeaderSize;
	}

	std::size_t Packet::Capacity() const
	{
		return _buffer.capacity();
	}

	bool Packet::DecodeHeader(UInt8* packetType, UInt32* size)
	{
		if (_buffer.size() < HeaderSize)
			return false;
		Stream headerStream(HeaderSize);
		headerStream.Write(_buffer.data(), HeaderSize);
		headerStream.SetCursorPos(0);
		headerStream >> _packetType >> _size;
		SetCursorPos(HeaderSize);
		_validHeader = true;
		if (packetType != nullptr)
			*packetType = _packetType;
		if (size != nullptr)
			*size = _size;
		return true;
	}

	bool Packet::EncodeHeader()
	{
		if (_buffer.size() < HeaderSize)
			return false;
		const UInt32 size = static_cast<UInt32>(_buffer.size()) - HeaderSize;
		Stream headerStream(HeaderSize);
		headerStream << _packetType << size;
		headerStream.SetCursorPos(0);
		headerStream.Read(_buffer.data(), HeaderSize);
		_validHeader = true;
		return true;
	}

	bool Packet::operator==(bool value) const
	{
		return _validHeader == value;
	}

	bool Packet::operator==(const Packet& value) const
	{
		if (GetDataSize() != value.GetDataSize())
			return false;
		return std::memcmp(GetData(), value.GetData(), GetDataSize()) == 0;
	}

	bool Packet::operator!=(bool value) const
	{
		return !operator==(value);
	}
}// namespace cct::net