//
// Created by arthur on 23/02/2023.
//

#include <cstring>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include "Concerto/Core/Network/Socket/Socket.hpp"
#include "Concerto/Core/Buffer/Buffer.hpp"

#ifdef CCT_PLATFORM_MACOS // because the CI is failing in release mode
#include <thread>
#endif

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;
	using namespace cct::net;

	SCENARIO("Socket - TCP server/client")
	{
		GIVEN("A TCP server listening on 127.0.0.1:8080 and a client that connects")
		{
			Socket::Initialize();
			const IpAddress ipAddress(127, 0, 0, 1, 8080);
			Socket server(SocketType::Tcp, IpProtocol::Ipv4);
			server.SetBlocking(true);
			server.Listen(ipAddress);

			Socket client(SocketType::Tcp, IpProtocol::Ipv4);
			client.Connect(ipAddress);

			const std::string helloWorld = "Hello World";
			Buffer buffer(11);
			std::memcpy(buffer.GetRawData(), helloWorld.c_str(), 11);
			client.Send(buffer);

			Socket serverClient(SocketType::Tcp, IpProtocol::Ipv4);
			serverClient.SetBlocking(true);
			REQUIRE(server.Accept(serverClient));

			Buffer buffer2(1024);

#ifdef CCT_PLATFORM_MACOS // because the CI is failing in release mode
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif

			WHEN("The server reads the data sent by the client")
			{
				const std::size_t availableBytes = serverClient.GetAvailableBytes();
				const std::size_t receivedSize = serverClient.Receive(buffer2);
				buffer2.Resize(receivedSize);

				THEN("11 bytes are available and the content matches the sent buffer")
				{
					REQUIRE(availableBytes == 11);
					REQUIRE(buffer2 == buffer);
				}
			}

			Socket::UnInitialize();
		}
	}

	SCENARIO("Socket - UDP server/client")
	{
		GIVEN("A UDP server bound to any:8080 and a client that sends data")
		{
			Socket::Initialize();
			Socket server(SocketType::Udp, IpProtocol::Ipv4);
			server.SetBlocking(true);
			auto ipAddress = IpAddress::AnyIPV4;
			ipAddress.SetPort(8080);
			REQUIRE(server.Bind(ipAddress));

			Socket client(SocketType::Udp, IpProtocol::Ipv4);
			IpAddress ip(127, 0, 0, 1, 8080);
			client.Connect(ip);

			std::string helloWorld = "Hello World";
			Buffer buffer(11);
			std::memcpy(buffer.GetRawData(), helloWorld.c_str(), 11);
			client.Send(buffer);

			WHEN("The server reads the data")
			{
				Buffer receivedBuffer(1024);
				std::size_t receivedSize = server.Receive(receivedBuffer);
				receivedBuffer.Resize(receivedSize);

				THEN("The received buffer matches the sent buffer") { REQUIRE(receivedBuffer == buffer); }
			}

			Socket::UnInitialize();
		}
	}

	SCENARIO("Socket - error handling")
	{
		GIVEN("A TCP server socket and a UDP client socket")
		{
			Socket::Initialize();
			Socket server(SocketType::Tcp, IpProtocol::Ipv4);
			Socket client(SocketType::Udp, IpProtocol::Ipv4);

			WHEN("Accept is called with a UDP client")
			{
				THEN("Accept returns false") { CHECK_FALSE(server.Accept(client)); }
			}

			Socket::UnInitialize();
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
