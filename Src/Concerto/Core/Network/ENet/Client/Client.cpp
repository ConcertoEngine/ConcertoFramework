//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#include "Concerto/Core/Network/ENet/Client/Client.hpp"
#include "Concerto/Core/Logger/Logger.hpp"
#include "Concerto/Core/Assert.hpp"
#include <enet/enet.h>

namespace cct::net
{
	EnetClient::EnetClient(UInt32 maxIncomingBandwidth, UInt32 maxOutgoingBandwidth) : 
		ENetHost(nullptr, 1, 2, maxIncomingBandwidth, maxOutgoingBandwidth),
		_peer(nullptr)
	{
	}

	bool EnetClient::SendPacket(const ENetPacket& packet, UInt8 channel, ENetPacket::Flag flags)
	{
		return ENetHost::SendPacket(packet, _peer.get(), channel, flags);
	}

	void EnetClient::Connect(const IpAddress& address)
	{
		ENetAddress enetAddress = {};
		const std::string addressStr = address.ToString();
		enet_address_set_host(&enetAddress, addressStr.c_str());
		enetAddress.port = address.GetPort();
		auto peer = enet_host_connect(static_cast<::ENetHost*>(_enetHost), &enetAddress, _maxChannels, 0);
		if (peer == nullptr)
		{
			CCT_ASSERT_FALSE("Cannot create an Enet connection");
			return;
		}
		_peer = std::make_unique<ENetPeer>(peer);
	}

	void EnetClient::Disconnect() const
	{
		CCT_ASSERT(_peer, "Invalid peer");
		_peer->Disconnect();
	}

}// namespace cct::net

#endif // CCT_ENABLE_ENET
