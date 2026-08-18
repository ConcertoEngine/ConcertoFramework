//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#ifndef CONCERTO_CORE_NETWORK_ENET_PEER_HPP
#define CONCERTO_CORE_NETWORK_ENET_PEER_HPP

#include <cstddef>

#include "Concerto/Core/Network/ENet/Packet/Packet.hpp"
#include "Concerto/Core/Types/Types.hpp"

namespace cct::net
{
	class Packet;
	class ENetHost;

	class CCT_CORE_PUBLIC_API ENetPeer
	{
	public:
		using ENetPeerHandle = void*;
		ENetPeer() = delete;
		explicit ENetPeer(ENetPeerHandle peer);
		ENetPeer(const ENetPeer&) = default;
		ENetPeer(ENetPeer&&) = default;
		~ENetPeer() = default;

		void Disconnect(UInt32 data = 0);
		void DisconnectNow(UInt32 data = 0);
		void DisconnectLater(UInt32 data = 0);

		void Ping();

		void SetTimeout(UInt32 timeoutLimit, UInt32 timeoutMinimum, UInt32 timeoutMaximum);
		void SetPingInterval(UInt32 pingInterval);
		bool SendPacket(const Packet& packet, UInt8 channel = 0, ENetPacket::Flag flags = ENetPacket::Flag::Reliable);
		bool SendPacket(const void* data, std::size_t size, UInt8 channel = 0, ENetPacket::Flag flags = ENetPacket::Flag::Reliable);

		[[nodiscard]] ENetPeerHandle GetHandle() const
		{
			return _peer;
		}

		ENetPeer& operator=(const ENetPeer&) = delete;
		ENetPeer& operator=(ENetPeer&&) = default;

	private:
		ENetPeerHandle _peer;
	};
} // namespace cct::net

#endif // CONCERTO_CORE_NETWORK_ENET_PEER_HPP

#endif // CCT_ENABLE_ENET