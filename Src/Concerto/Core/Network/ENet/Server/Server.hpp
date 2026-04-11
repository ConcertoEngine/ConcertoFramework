//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#ifndef CONCERTO_CORE_NETWORK_ENET_SERVER_HPP
#define CONCERTO_CORE_NETWORK_ENET_SERVER_HPP

#include <functional>

#include "Concerto/Core/Network/IpAddress/IpAddress.hpp"
#include "Concerto/Core/Network/ENet/Host/Host.hpp"

namespace cct::net
{
	class CCT_CORE_PUBLIC_API EnetServer : public ENetHost
	{
	 public:
		EnetServer() = delete;
		explicit EnetServer(IpAddress address, std::size_t maxClients = 32, UInt32 maxIncomingBandwidth = 0, UInt32 maxOutgoingBandwidth = 0);
	private:
		IpAddress _address;
	};
}// namespace cct::net

#endif // CONCERTO_CORE_NETWORK_ENET_SERVER_HPP

#endif // CCT_ENABLE_ENET