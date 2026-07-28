//
// Created by arthur on 27/05/2023.
//

#include "Concerto/Core/Network/IpAddress/IpAddress.hpp"

#include <cstring>

#ifdef CCT_PLATFORM_WINDOWS
#include <WinSock2.h>
#include <ws2tcpip.h>
#undef SetPort
#elif defined(CCT_PLATFORM_POSIX)
#include <arpa/inet.h>
#endif

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Logger/Logger.hpp"

namespace cct::net
{
	namespace
	{
		bool ParseIpV4(std::string_view ip, IpAddress::IPv4& out)
		{
			in_addr addr = {};
			const std::string text(ip);
			if (inet_pton(AF_INET, text.c_str(), &addr) != 1)
				return false;
			std::memcpy(out.data(), &addr, out.size());
			return true;
		}

		bool ParseIpV6(std::string_view ip, IpAddress::IPv6& out)
		{
			in6_addr addr = {};
			const std::string text(ip);
			if (inet_pton(AF_INET6, text.c_str(), &addr) != 1)
				return false;

			for (std::size_t i = 0; i < out.size(); ++i)
				out[i] = static_cast<UInt16>((addr.s6_addr[i * 2] << 8) | addr.s6_addr[i * 2 + 1]);
			return true;
		}

		std::string FormatIpV6(const IpAddress::IPv6& groups)
		{
			in6_addr addr = {};
			for (std::size_t i = 0; i < groups.size(); ++i)
			{
				addr.s6_addr[i * 2] = static_cast<UInt8>(groups[i] >> 8);
				addr.s6_addr[i * 2 + 1] = static_cast<UInt8>(groups[i] & 0xFF);
			}

			char buffer[INET6_ADDRSTRLEN] = {};
			if (inet_ntop(AF_INET6, &addr, buffer, sizeof(buffer)) == nullptr)
				return {};
			return buffer;
		}
	} // namespace

	const IpAddress IpAddress::AnyIPV4 = IpAddress(0, 0, 0, 0, 0);
	const IpAddress IpAddress::AnyIPV6 = IpAddress(IpAddress::IPv6{}, 0);

	IpAddress::IpAddress(const IpAddress::IPv4& ip, UInt16 port) :
		_ipv4(ip),
		_protocol(IpProtocol::Ipv4),
		_port(port)
	{
	}
	IpAddress::IpAddress(const IpAddress::IPv6& ip, UInt16 port) :
		_ipv6(ip),
		_protocol(IpProtocol::Ipv6),
		_port(port)
	{
	}

	IpAddress::IpAddress(UInt8 a, UInt8 b, UInt8 c, UInt8 d, UInt16 port) :
		_ipv4({a, b, c, d}),
		_protocol(IpProtocol::Ipv4),
		_port(port)
	{
	}

	IpAddress::IpAddress(UInt32 address, UInt16 port) :
		_ipv4({static_cast<UInt8>(address >> 24),
			   static_cast<UInt8>(address >> 16),
			   static_cast<UInt8>(address >> 8),
			   static_cast<UInt8>(address)}),
		_protocol(IpProtocol::Ipv4),
		_port(port)
	{
	}

	IpAddress::IpAddress(std::string_view ip, UInt16 port) :
		_protocol(IpProtocol::Error),
		_port(port)
	{
		IPv4 v4{};
		IPv6 v6{};
		if (ParseIpV4(ip, v4))
		{
			_ipv4 = v4;
			_protocol = IpProtocol::Ipv4;
			return;
		}
		if (ParseIpV6(ip, v6))
		{
			_ipv6 = v6;
			_protocol = IpProtocol::Ipv6;
			return;
		}
		CCT_ASSERT_FALSE("Cannot parse Ip address '{}'", ip);
	}

	IpProtocol IpAddress::GetProtocol() const
	{
		return _protocol;
	}

	const IpAddress::IPv4& IpAddress::GetIPv4() const
	{
		CCT_ASSERT(_protocol == IpProtocol::Ipv4, "Invalid Ip protocol");
		return _ipv4;
	}

	const IpAddress::IPv6& IpAddress::GetIPv6() const
	{
		CCT_ASSERT(_protocol == IpProtocol::Ipv6, "Invalid Ip protocol");
		return _ipv6;
	}

	UInt16 IpAddress::GetPort() const
	{
		return _port;
	}

	void IpAddress::SetPort(UInt16 port)
	{
		_port = port;
	}

	UInt32 IpAddress::ToUInt32() const
	{
		CCT_ASSERT(_protocol == IpProtocol::Ipv4, "Invalid Ip protocol");
		return (_ipv4[0] << 24) | (_ipv4[1] << 16) | (_ipv4[2] << 8) | _ipv4[3];
	}

	std::string IpAddress::ToString() const
	{
		if (_protocol == IpProtocol::Ipv4)
			return std::to_string(_ipv4[0]) + "." + std::to_string(_ipv4[1]) + "." + std::to_string(_ipv4[2]) + "." + std::to_string(_ipv4[3]);
		if (_protocol == IpProtocol::Ipv6)
			return FormatIpV6(_ipv6);

		CCT_ASSERT_FALSE("Invalid Ip protocol");
		return {};
	}

	bool IpAddress::IsAny() const
	{
		switch (_protocol)
		{
			case IpProtocol::Any:
				return true;
			case IpProtocol::Ipv4:
				return _ipv4 == IPv4{};
			case IpProtocol::Ipv6:
				return _ipv6 == IPv6{};
			default:
				return false;
		}
	}

	bool IpAddress::IsIpV4(std::string_view ip)
	{
		IPv4 v4{};
		return ParseIpV4(ip, v4);
	}

	bool IpAddress::IsIpV6(std::string_view ip)
	{
		IPv6 v6{};
		return ParseIpV6(ip, v6);
	}

	IpProtocol IpAddress::DetectProtocol(std::string_view ip)
	{
		if (IsIpV4(ip))
			return IpProtocol::Ipv4;
		else if (IsIpV6(ip))
			return IpProtocol::Ipv6;
		return IpProtocol::Error;
	}
} // namespace cct::net