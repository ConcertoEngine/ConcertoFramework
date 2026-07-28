//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#include "Concerto/Core/Network/ENet/Server/Server.hpp"

#include <enet6/enet.h>

namespace cct::net
{
	EnetServer::EnetServer(IpAddress address, std::size_t maxClients, UInt32 maxIncomingBandwidth, UInt32 maxOutgoingBandwidth, IpProtocol protocol) :
		ENetHost(&address, maxClients, 2, maxIncomingBandwidth, maxOutgoingBandwidth, protocol),
		_address(address)
	{
	}
} // namespace cct::net

#endif // CCT_ENABLE_ENET