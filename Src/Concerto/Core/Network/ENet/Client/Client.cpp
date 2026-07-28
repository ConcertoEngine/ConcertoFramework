//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#include "Concerto/Core/Network/ENet/Client/Client.hpp"

#include <string>

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Logger/Logger.hpp"
#include "Concerto/Core/Network/ENet/Address/Address.hpp"

namespace cct::net
{
	EnetClient::EnetClient(UInt32 maxIncomingBandwidth, UInt32 maxOutgoingBandwidth, IpProtocol protocol) :
		ENetHost(nullptr, 1, 2, maxIncomingBandwidth, maxOutgoingBandwidth, protocol),
		_peer(nullptr)
	{
	}

	bool EnetClient::SendPacket(const ENetPacket& packet, UInt8 channel, ENetPacket::Flag flags)
	{
		return ENetHost::SendPacket(packet, _peer.get(), channel, flags);
	}

	bool EnetClient::Connect(const IpAddress& address)
	{
		::ENetAddress enetAddress = {};
		if (!ToENetAddress(address, ToENetAddressType(_protocol), enetAddress))
		{
			CCT_ASSERT_FALSE("Address '{}' is not reachable from this client", address.ToString());
			return false;
		}

		auto peer = enet_host_connect(static_cast<::ENetHost*>(_enetHost), &enetAddress, _maxChannels, 0);
		if (peer == nullptr)
		{
			CCT_ASSERT_FALSE("Cannot create an Enet connection");
			return false;
		}
		_peer = std::make_unique<ENetPeer>(peer);
		return true;
	}

	bool EnetClient::Connect(std::string_view hostName, UInt16 port)
	{
		::ENetAddress enetAddress = {};
		const std::string name(hostName);
		if (enet_address_set_host(&enetAddress, ToENetAddressType(_protocol), name.c_str()) != 0)
		{
			CCT_ASSERT_FALSE("Cannot resolve host '{}'", hostName);
			return false;
		}

		enetAddress.port = port;

		IpAddress resolved;
		if (!FromENetAddress(enetAddress, resolved))
			return false;
		return Connect(resolved);
	}

	void EnetClient::Disconnect() const
	{
		CCT_ASSERT(_peer, "Invalid peer");
		_peer->Disconnect();
	}

} // namespace cct::net

#endif // CCT_ENABLE_ENET
