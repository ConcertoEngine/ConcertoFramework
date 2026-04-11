//
// Created by arthur on 09/08/2023.
//

#include <catch2/catch_test_macros.hpp>
#include "Concerto/Core/Network/IpAddress/IpAddress.hpp"

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;
	using namespace cct::net;
	static constexpr UInt32 LocalHost = 2130706433;
	static constexpr IpAddress::IPv4 LocalHostArray = {127, 0, 0, 1};

	SCENARIO("IpAddress - ParseIPV4")
	{
		GIVEN("LocalHost constructed from UInt32")
		{
			IpAddress ip(LocalHost, 2121);
			THEN("ToUInt32, GetProtocol and GetIPv4 are correct")
			{
				CHECK(ip.ToUInt32() == LocalHost);
				CHECK(ip.GetProtocol() == IpProtocol::Ipv4);
				CHECK(ip.GetIPv4() == LocalHostArray);
			}
		}

		GIVEN("LocalHost constructed from string \"127.0.0.1\"")
		{
			IpAddress ip("127.0.0.1", 2121);
			THEN("ToUInt32, GetIPv4 and GetProtocol are correct")
			{
				CHECK(ip.ToUInt32() == LocalHost);
				CHECK(ip.GetIPv4() == LocalHostArray);
				CHECK(ip.GetProtocol() == IpProtocol::Ipv4);
			}
		}

		GIVEN("LocalHost constructed from IPv4 array")
		{
			IpAddress ip(LocalHostArray, 2121);
			THEN("ToUInt32, GetIPv4 and GetProtocol are correct")
			{
				CHECK(ip.ToUInt32() == LocalHost);
				CHECK(ip.GetIPv4() == LocalHostArray);
				CHECK(ip.GetProtocol() == IpProtocol::Ipv4);
			}
		}

		GIVEN("An invalid IP string \"xxx.x.x.x\"")
		{
			IpAddress ip("xxx.x.x.x", 2121);
			THEN("Protocol is Error") { CHECK(ip.GetProtocol() == IpProtocol::Error); }
		}
	}

	SCENARIO("IpAddress - IsIpV4")
	{
		THEN("Valid IPv4 addresses are recognized") { CHECK(IpAddress::IsIpV4("127.0.0.1")); }

		THEN("Invalid IPv4 addresses are rejected")
		{
			CHECK_FALSE(IpAddress::IsIpV4("1277.0.0.1"));
			CHECK_FALSE(IpAddress::IsIpV4("127.1277.0.1"));
			CHECK_FALSE(IpAddress::IsIpV4("127.0.1277.1"));
			CHECK_FALSE(IpAddress::IsIpV4("127.0.127.1277"));
			CHECK_FALSE(IpAddress::IsIpV4("127..0.1"));
			CHECK_FALSE(IpAddress::IsIpV4("0:0:0:0:0:0:0:1"));
		}
	}

	SCENARIO("IpAddress - IsIpV6")
	{
		THEN("Valid IPv6 addresses are recognized")
		{
			CHECK(IpAddress::IsIpV6("0:0:0:0:0:0:0:1"));
			CHECK(IpAddress::IsIpV6("::1"));
			CHECK(IpAddress::IsIpV6("2001:db8:3333:4444:5555:6666:7777:8888"));
			CHECK(IpAddress::IsIpV6("2001:0db8:0001:0000:0000:0ab9:C0A8:010"));
			CHECK(IpAddress::IsIpV6("fe80:0000:0000:0000:0000:0000:0000:1"));
			CHECK(IpAddress::IsIpV6("2001:db8::1234:5678"));
			CHECK(IpAddress::IsIpV6("::1234:5678"));
			CHECK(IpAddress::IsIpV6("2001:db8::"));
		}

		THEN("Invalid IPv6 addresses are rejected")
		{
			CHECK_FALSE(IpAddress::IsIpV6("192.168.1.1"));
			CHECK_FALSE(IpAddress::IsIpV6("2001:0db8:85a3:0000:0000:8a2e:0370:7334:1234"));
			CHECK_FALSE(IpAddress::IsIpV6("fe80:::1"));
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
