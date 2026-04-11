//
// Created by arthur on 28/05/2023.
//

#ifndef CONCERTO_CORE_NETWORK_PACKET_HPP
#define CONCERTO_CORE_NETWORK_PACKET_HPP

#include <cstddef>
#include "Concerto/Core/Types/Types.hpp"
#include "Concerto/Core/Stream/Stream.hpp"

namespace cct::net
{
	class CCT_CORE_PUBLIC_API Packet : public Stream
	{
	 public:
		Packet();
		Packet(UInt8 packetType, const void* data, UInt32 size);
		explicit Packet(UInt8 packetType, std::size_t capacity = 0);

		[[nodiscard]] UInt8 GetPacketType() const;
		[[nodiscard]] const Byte* GetData() const;
		[[nodiscard]] UInt64 GetDataSize() const;
		[[nodiscard]] std::size_t Capacity() const;

		[[nodiscard]] bool DecodeHeader(UInt8* packetType = nullptr, UInt32* size = nullptr);
		[[nodiscard]] bool EncodeHeader();

		bool operator==(bool) const;
		bool operator==(const Packet&) const;
		bool operator!=(bool) const;

		template<typename T>
		Packet& operator<<(T value)
		{
			Stream::operator<<(value);
			_size += sizeof(T);
			return *this;
		}

		static constexpr UInt32 HeaderSize = sizeof(UInt8) + sizeof(UInt32);
	 private:
		UInt32 _size;
		UInt8 _packetType;
		bool _validHeader;
	};
}

#endif //CONCERTO_CORE_NETWORK_PACKET_HPP
