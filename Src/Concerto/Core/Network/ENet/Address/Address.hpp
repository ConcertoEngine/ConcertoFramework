//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#ifndef CONCERTO_CORE_NETWORK_ENET_ADDRESS_HPP
#define CONCERTO_CORE_NETWORK_ENET_ADDRESS_HPP

#include "Concerto/Core/Network/IpAddress/IpAddress.hpp"
#include <enet6/enet.h>

namespace cct::net
{
	::ENetAddressType ToENetAddressType(IpProtocol protocol);

	bool ToENetAddress(const IpAddress& address, ::ENetAddressType hostType, ::ENetAddress& out);

	bool FromENetAddress(const ::ENetAddress& address, IpAddress& out);
} // namespace cct::net

#endif // CONCERTO_CORE_NETWORK_ENET_ADDRESS_HPP

#endif // CCT_ENABLE_ENET
