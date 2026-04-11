//
// Created by arthur on 13/08/2023.
//

#ifdef CCT_ENABLE_ENET

#include <thread>
#include <chrono>
#include <span>

#include <catch2/catch_test_macros.hpp>
#include <Concerto/Core/Logger/Logger.hpp>
#include <Concerto/Core/Network/ENet/ENet.hpp>
#include <Concerto/Core/Network/ENet/Server/Server.hpp>
#include <Concerto/Core/Network/ENet/Client/Client.hpp>

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;
	using namespace cct::net;

	SCENARIO("Enet - BasicConnection")
	{
		GIVEN("An ENet server listening on port 2121")
		{
			ENet::Initialize();
			bool running = true;
			std::thread serverThread([&]() {
				IpAddress listeningIp("0.0.0.0", 2121);
				EnetServer server(listeningIp);
				Int32 count = -1;
				ENetEvent event;
				while (running)
				{
					Int32 ret = server.PollEvent(&event, 100);
					if (ret <= 0)
						continue;
					if (event.eventType == ENetEvent::Type::Connect)
						count += 2;
					else if (event.eventType == ENetEvent::Type::Disconnect)
						count--;
				}
				REQUIRE(count == 0);
			});
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			WHEN("A client connects then disconnects")
			{
				EnetClient client;
				IpAddress ip("127.0.0.1", 2121);
				Int32 callbackCount = 0;
				client.Connect(ip);
				ENetEvent event;
				Int32 ret = client.PollEvent(&event, 100);
				REQUIRE(ret > 0);
				REQUIRE(event.eventType == ENetEvent::Type::Connect);

				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				ret = client.PollEvent(&event, 100);
				client.Disconnect();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				ret = client.PollEvent(&event, 100);

				THEN("The disconnect event is received and no unexpected callbacks fired")
				{
					REQUIRE(ret > 0);
					REQUIRE(event.eventType == ENetEvent::Type::Disconnect);
					CHECK(callbackCount == 0);
				}
			}

			running = false;
			serverThread.join();
			ENet::Deinitialize();
		}
	}

	SCENARIO("Enet - SendingPacket")
	{
		GIVEN("An ENet server listening on port 2121")
		{
			ENet::Initialize();
			bool running = true;
			constexpr UInt8 PacketType = 0xF;
			std::thread serverThread([&]() {
				IpAddress listeningIp("0.0.0.0", 2121);
				EnetServer server(listeningIp);
				ENetEvent event;
				while (running)
				{
					Int32 ret = server.PollEvent(&event, 100);
					if (ret <= 0)
						continue;
					if (event.eventType == ENetEvent::Type::Receive)
					{
						ENetPacket& packet = *event.packet;
						UInt8 packetType = 0;
						packet >> packetType;
						REQUIRE(PacketType == packetType);
						Int32 v42, v84;
						packet >> v42 >> v84;
						REQUIRE(v42 == 42);
						REQUIRE(v84 == 84);
						REQUIRE(server.SendPacket(packet, event.peer.get()));
					}
				}
			});
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			WHEN("A client sends a packet with PacketType, 42, 84")
			{
				EnetClient client;
				IpAddress ip("127.0.0.1", 2121);
				Int32 callbackCount = 0;
				client.Connect(ip);
				ENetEvent event;
				Int32 ret = client.PollEvent(&event, 100);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				ENetPacket packet;
				packet << PacketType << Int32(42) << Int32(84);
				REQUIRE(client.SendPacket(packet));
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				ret = client.PollEvent(&event, 100);

				THEN("The server echoes it back correctly")
				{
					if (event.eventType == ENetEvent::Type::Receive)
					{
						ENetPacket& newPacket = *event.packet;
						CHECK(packet.GetSize() == newPacket.GetSize());
						CHECK(packet == newPacket);
					}
					else
					{
						FAIL("Expected Receive event");
					}
				}
			}

			running = false;
			serverThread.join();
			ENet::Deinitialize();
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE

#endif // CCT_ENABLE_ENET
