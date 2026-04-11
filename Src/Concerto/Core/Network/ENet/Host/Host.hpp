//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#ifndef CONCERTO_CORE_NETWORK_ENET_HOST_HPP
#define CONCERTO_CORE_NETWORK_ENET_HOST_HPP

#include "Concerto/Core/Types/Types.hpp"
#include "Concerto/Core/Network/IpAddress/IpAddress.hpp"
#include "Concerto/Core/Network/ENet/Event.hpp"
#include "Concerto/Core/Network/ENet/Packet/Packet.hpp"

namespace cct::net
{
	class CCT_CORE_PUBLIC_API ENetHost
	{
	 public:
		using Handle = void*;
		explicit ENetHost(IpAddress* address = nullptr, std::size_t maxConnections = 1, std::size_t maxChannels = 2, UInt32 maxIncomingBandwidth = 0, UInt32 maxOutgoingBandwidth = 0);
		ENetHost(const ENetHost&) = delete;
		ENetHost(ENetHost&&) = default;
		virtual ~ENetHost();


		virtual Int32 PollEvent(ENetEvent* event, UInt32 timeout = 0);
		bool SendPacket(const void* data, std::size_t size, ENetPeer* peer, UInt8 channel = 0, ENetPacket::Flag flags = ENetPacket::Flag::Reliable);
		bool SendPacket(const ENetPacket& packet, ENetPeer* peer, UInt8 channel = 0, ENetPacket::Flag flags = ENetPacket::Flag::Reliable);
		void Flush();

		ENetHost& operator=(const ENetHost&) = delete;
		ENetHost& operator=(ENetHost&&) = default;
	 protected:
		bool CreateHost(IpAddress* address = nullptr);

		Handle _enetHost;
		std::size_t _maxConnections;
		std::size_t _maxChannels;
		UInt32 _maxIncomingBandwidth;
		UInt32 _maxOutgoingBandwidth;
	};
}// namespace cct::net

#endif//CONCERTO_CORE_NETWORK_HOST_CLIENT_HPP

#endif // CCT_ENABLE_ENET