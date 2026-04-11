//
// Created by arthur on 14/08/2023.
//

#ifdef CCT_ENABLE_ENET

#ifndef CONCERTO_CORE_NETWORK_ENET_PACKET_HPP
#define CONCERTO_CORE_NETWORK_ENET_PACKET_HPP

#include "Concerto/Core/Types/Types.hpp"
#include "Concerto/Core/Stream/Stream.hpp"

namespace cct::net
{
	class CCT_CORE_PUBLIC_API ENetPacket : public Stream
	{
	public:
		enum Flag
		{
			Reliable,
			Unsequenced,
			NoAllocate,
			UnreliableFragment,
			Sent
		};

	   ENetPacket();
	   ENetPacket(const void* data, std::size_t size);

	   [[nodiscard]] const Byte* GetData() const;
	   [[nodiscard]] std::size_t Capacity() const;

	   bool operator==(const ENetPacket&) const;
	   bool operator!=(const ENetPacket&) const;
	};
}// namespace cct::net::ENetPacket

#endif//CONCERTO_CORE_NETWORK_ENET_PACKET_HPP

#endif // CCT_ENABLE_ENET