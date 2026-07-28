//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET
#include "Concerto/Core/Network/ENet/Address/Address.hpp"

#include <algorithm>
#include <cstring>

namespace cct::net
{
	::ENetAddressType ToENetAddressType(IpProtocol protocol)
	{
		switch (protocol)
		{
			case IpProtocol::Ipv4:
				return ENET_ADDRESS_TYPE_IPV4;
			case IpProtocol::Ipv6:
				return ENET_ADDRESS_TYPE_IPV6;
			default:
				return ENET_ADDRESS_TYPE_ANY;
		}
	}

	bool ToENetAddress(const IpAddress& address, ::ENetAddressType hostType, ::ENetAddress& out)
	{
		const ::ENetAddressType wireType = (hostType == ENET_ADDRESS_TYPE_ANY) ? ENET_ADDRESS_TYPE_IPV6 : hostType;

		if (address.IsAny())
		{
			enet_address_build_any(&out, wireType);
			out.port = address.GetPort();
			return true;
		}

		switch (address.GetProtocol())
		{
			case IpProtocol::Ipv4:
			{
				const IpAddress::IPv4& ip = address.GetIPv4();
				out.type = ENET_ADDRESS_TYPE_IPV4;
				std::memcpy(out.host.v4, ip.data(), ip.size());
				break;
			}
			case IpProtocol::Ipv6:
			{
				const IpAddress::IPv6& ip = address.GetIPv6();
				out.type = ENET_ADDRESS_TYPE_IPV6;
				std::copy(ip.begin(), ip.end(), out.host.v6);
				break;
			}
			default:
				return false;
		}
		out.port = address.GetPort();

		if (out.type == ENET_ADDRESS_TYPE_IPV4 && wireType == ENET_ADDRESS_TYPE_IPV6)
			enet_address_convert_ipv6(&out);

		return out.type == wireType;
	}

	bool FromENetAddress(const ::ENetAddress& address, IpAddress& out)
	{
		switch (address.type)
		{
			case ENET_ADDRESS_TYPE_IPV4:
			{
				IpAddress::IPv4 ip{};
				std::memcpy(ip.data(), address.host.v4, ip.size());
				out = IpAddress(ip, address.port);
				return true;
			}
			case ENET_ADDRESS_TYPE_IPV6:
			{
				IpAddress::IPv6 ip{};
				std::copy(std::begin(address.host.v6), std::end(address.host.v6), ip.begin());
				out = IpAddress(ip, address.port);
				return true;
			}
			default:
				return false;
		}
	}
} // namespace cct::net

#endif // CCT_ENABLE_ENET
