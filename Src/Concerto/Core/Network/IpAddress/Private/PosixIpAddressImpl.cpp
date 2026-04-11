//
// Created by arthur on 28/05/2023.
//
#include "Concerto/Core/Types/Types.hpp"
#ifdef CCT_PLATFORM_POSIX

#include <sys/socket.h>
#include <stdexcept>

#include "Concerto/Core/Network/IpAddress/Private/PosixIpAddressImpl.hpp"

namespace cct::net
{
	IpAddress cct::net::IpAddressImpl::FromSockAddr(const sockaddr* addr)
	{
		if (addr->sa_family == AF_INET)
		{
			const auto sockaddIn = reinterpret_cast<const sockaddr_in*>(addr);
			const UInt32 ipv4 = sockaddIn->sin_addr.s_addr;
			return IpAddress{ ipv4, ntohs(sockaddIn->sin_port) };
		}
		else
		{
			throw std::runtime_error("Not implemented");
		}
	}

	sockaddr_in IpAddressImpl::ToSockAddr(const IpAddress& address)
	{
		if (address.GetProtocol() != IpProtocol::Ipv4)
			throw std::runtime_error("Not implemented");
		sockaddr_in addr = {};
		UInt32 ipv4 = address.ToUInt32();
		addr.sin_family = AF_INET;
		addr.sin_port = htons(address.GetPort());
		addr.sin_addr.s_addr = htonl(ipv4);
		return addr;
	}
}
#endif