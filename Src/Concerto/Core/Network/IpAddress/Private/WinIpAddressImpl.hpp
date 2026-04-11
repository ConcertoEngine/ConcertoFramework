//
// Created by arthur on 26/05/2023.
//

#ifndef CONCERTO_CORE_SRC_CONCERTO_CORE_NETWORK_IPADRESS_IPADRESS_HPP_
#define CONCERTO_CORE_SRC_CONCERTO_CORE_NETWORK_IPADRESS_IPADRESS_HPP_
#include "Concerto/Core/Types/Types.hpp"
#ifdef CCT_PLATFORM_WINDOWS
#include <WinSock2.h>
#include "Concerto/Core/Network/IpAddress/IpAddress.hpp"

namespace cct::net
{
	class IpAddressImpl
	{
	 public:
		IpAddressImpl() = delete;
		~IpAddressImpl() = delete;
		static IpAddress FromSockAddr(const sockaddr* addr);
		static sockaddr_in ToSockAddr(const IpAddress& address);
	};
}
#endif
#endif //CONCERTO_CORE_SRC_CONCERTO_CORE_NETWORK_IPADRESS_IPADRESS_HPP_
