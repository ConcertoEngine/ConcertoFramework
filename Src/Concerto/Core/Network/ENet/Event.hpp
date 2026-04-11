//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#ifndef CONCERTO_CORE_NETWORK_ENET_EVENT_HPP
#define CONCERTO_CORE_NETWORK_ENET_EVENT_HPP

#include <memory>
#include "Concerto/Core/Network/ENet/Packet/Packet.hpp"
#include "Concerto/Core/Network/ENet/Peer/Peer.hpp"

namespace cct::net
{
	struct CCT_CORE_PUBLIC_API ENetEvent
	{
		enum Type 
		{
			None,
			Connect,
			Disconnect,
			Receive
		};

		Type eventType;
		std::unique_ptr<ENetPeer> peer;
		UInt8 channelId;
		UInt32 data;
		std::unique_ptr<ENetPacket> packet;
	};
}

#endif // CONCERTO_CORE_NETWORK_ENET_EVENT_HPP

#endif // CCT_ENABLE_ENET