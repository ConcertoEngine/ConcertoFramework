//
// Created by arthur on 25/05/2023.
//

#ifndef CONCERTO_CORE_NETWORK_IPADRESS_HPP
#define CONCERTO_CORE_NETWORK_IPADRESS_HPP

#include <array>
#include <string_view>
#include <string>
#include "Concerto/Core/Types/Types.hpp"

namespace cct::net
{
	enum class IpProtocol
	{
		Error = -1,
		Ipv4,
		Ipv6,
		Any = Ipv4 //Fixme
	};

	class CCT_CORE_PUBLIC_API IpAddress
	{
	 public:
		using IPv4 = std::array<UInt8, 4>;
		using IPv6 = std::array<UInt16, 16>;

		IpAddress() = default;
		IpAddress(const IPv4& ip, UInt16 port);
		IpAddress(const IPv6& ip, UInt16 port);
		IpAddress(std::string_view ip, UInt16 port);
		IpAddress(UInt8 a, UInt8 b, UInt8 c, UInt8 d, UInt16 port);
		IpAddress(UInt32 address, UInt16 port);

		[[nodiscard]] IpProtocol GetProtocol() const;
		[[nodiscard]] const IPv4& GetIPv4() const;
		[[nodiscard]] const IPv6& GetIPv6() const;
		[[nodiscard]] UInt16 GetPort() const;

		[[nodiscard]] UInt32 ToUInt32() const;
		[[nodiscard]] std::string ToString() const;

		static bool IsIpV4(std::string_view ip);
		static bool IsIpV6(std::string_view ip);

		static IpProtocol DetectProtocol(std::string_view ip);

		void SetPort(UInt16 port);

		static const IpAddress AnyIPV4;
	 private:
		union
		{
			IPv4 _ipv4;
			IPv6 _ipv6;
		};
		IpProtocol _protocol;
		UInt16 _port;
	};
}
#endif //CONCERTO_CORE_NETWORK_IPADRESS_HPP
