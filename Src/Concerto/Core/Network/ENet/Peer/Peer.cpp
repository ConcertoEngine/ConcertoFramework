//
// Created by arthur on 14/08/2023.
//

#ifdef CCT_ENABLE_ENET

#include "Concerto/Core/Network/ENet/Peer/Peer.hpp"
#include "Concerto/Core/Network/Packet/Packet.hpp"
#include <enet/enet.h>

namespace cct::net
{
	ENetPeer::ENetPeer(ENetPeerHandle peer)
		: _peer(peer)
	{
	}

	void ENetPeer::Disconnect(UInt32 data)
	{
		enet_peer_disconnect(static_cast<::ENetPeer*>(_peer), data);
	}

	void ENetPeer::DisconnectNow(UInt32 data)
	{
		enet_peer_disconnect_now(static_cast<::ENetPeer*>(_peer), data);
	}

	void ENetPeer::DisconnectLater(UInt32 data)
	{
		enet_peer_disconnect_later(static_cast<::ENetPeer*>(_peer), data);
	}

	void ENetPeer::SetTimeout(UInt32 timeoutLimit, UInt32 timeoutMinimum, UInt32 timeoutMaximum)
	{
		enet_peer_timeout(static_cast<::ENetPeer*>(_peer), timeoutLimit, timeoutMinimum, timeoutMaximum);
	}

	void ENetPeer::Ping()
	{
		enet_peer_ping(static_cast<::ENetPeer*>(_peer));
	}

	void ENetPeer::SetPingInterval(UInt32 pingInterval)
	{
		enet_peer_ping_interval(static_cast<::ENetPeer*>(_peer), pingInterval);
	}

	bool ENetPeer::SendPacket(const Packet& packet, UInt8 channel, ENetPacket::Flag flags)
	{
		return SendPacket(packet.GetData(), packet.GetSize(), channel, flags) == 0;
	}

	bool ENetPeer::SendPacket(const void* data, std::size_t size, UInt8 channel, ENetPacket::Flag flags)
	{
		::ENetPacket* enetPacket = enet_packet_create(data, size, flags);
		return enet_peer_send(static_cast<::ENetPeer*>(_peer), channel, enetPacket) == 0;
	}
}

#endif // CCT_ENABLE_ENET