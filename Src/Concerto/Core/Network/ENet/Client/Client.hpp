//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#ifndef CONCERTO_CORE_NETWORK_ENET_CLIENT_HPP
#define CONCERTO_CORE_NETWORK_ENET_CLIENT_HPP

#include <functional>
#include <memory>

#include "Concerto/Core/Network/ENet/Host/Host.hpp"

namespace cct::net
{
	class CCT_CORE_PUBLIC_API EnetClient : public ENetHost
	{
	 public:
		EnetClient(UInt32 maxIncomingBandwidth = 0, UInt32 maxOutgoingBandwidth = 0);

		bool SendPacket(const ENetPacket& packet, UInt8 channel = 0, ENetPacket::Flag flags = ENetPacket::Flag::Reliable);
		void Connect(const IpAddress& address);
		void Disconnect() const;
	private:
		std::unique_ptr<ENetPeer> _peer;
	};
}

#endif // CONCERTO_CORE_NETWORK_ENET_CLIENT_HPP

#endif // CCT_ENABLE_ENET