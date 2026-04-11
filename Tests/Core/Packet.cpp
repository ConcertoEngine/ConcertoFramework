//
// Created by arthur on 30/05/2023.
//

#include <catch2/catch_test_macros.hpp>
#include <array>
#include "Concerto/Core/Network/Packet/Packet.hpp"

namespace CCT_ANONYMOUS_NAMESPACE
{
	using namespace cct;
	using namespace cct::net;

	constexpr UInt8 PacketType = 0xF;
	const UInt8 ByteSwappedPacketType = ByteSwap(PacketType);

	struct PacketHeader
	{
		UInt8 PacketType;
		UInt32 Size;
	};

	SCENARIO("Packet - construction")
	{
		GIVEN("A Packet constructed from PacketType and no data")
		{
			Packet packet(PacketType, nullptr, 0);
			THEN("Sizes and type are correct")
			{
				CHECK(packet.GetPacketType() == PacketType);
				CHECK(packet.GetSize() == Packet::HeaderSize);
				CHECK(packet.GetDataSize() == 0);
				CHECK(packet.Capacity() == Packet::HeaderSize);
			}
		}

		GIVEN("A Packet with reserved capacity of 5")
		{
			Packet packet2(PacketType, 5);
			THEN("Capacity includes the reserved space")
			{
				CHECK(packet2.GetPacketType() == PacketType);
				CHECK(packet2.GetSize() == Packet::HeaderSize);
				CHECK(packet2.GetDataSize() == 0);
				CHECK(packet2.Capacity() == Packet::HeaderSize + 5);
			}
		}

		GIVEN("A default-constructed Packet")
		{
			Packet packet3;
			THEN("Type is 0 and sizes are zero/HeaderSize")
			{
				CHECK(packet3.GetPacketType() == 0);
				CHECK(packet3.GetSize() == 0);
				CHECK(packet3.GetSize() == 0);
				CHECK(packet3.Capacity() == Packet::HeaderSize);
			}
		}
	}

	SCENARIO("Packet - DecodeHeader")
	{
		GIVEN("A Packet from PacketType with no data")
		{
			PacketHeader header = {};
			Packet packet(PacketType, nullptr, 0);

			WHEN("Header is encoded then decoded")
			{
				CHECK(packet.EncodeHeader());
				CHECK(packet.DecodeHeader(&header.PacketType, &header.Size));
				THEN("Header fields match")
				{
					CHECK(header.PacketType == ByteSwappedPacketType);
					CHECK(header.Size == 0);
				}
			}
		}

		GIVEN("A Packet with reserved capacity of 5")
		{
			PacketHeader header = {};
			Packet packet2(PacketType, 5);

			WHEN("Header is encoded then decoded")
			{
				CHECK(packet2.EncodeHeader());
				CHECK(packet2.DecodeHeader(&header.PacketType, &header.Size));
				THEN("Header fields match")
				{
					CHECK(header.PacketType == ByteSwappedPacketType);
					CHECK(header.Size == 0);
				}
			}
		}

		GIVEN("A default-constructed (invalid) Packet")
		{
			PacketHeader header = {};
			Packet packet3;

			WHEN("EncodeHeader/DecodeHeader are called")
			{
				THEN("Both return false")
				{
					CHECK_FALSE(packet3.EncodeHeader());
					CHECK_FALSE(packet3.DecodeHeader(&header.PacketType, &header.Size));
				}
			}
		}

		GIVEN("A Packet with raw data \"Hello\"")
		{
			PacketHeader header = {};
			const char* data = "Hello";
			Packet packet4(PacketType, data, 5);

			WHEN("Header is encoded then decoded")
			{
				CHECK(packet4.EncodeHeader());
				CHECK(packet4.DecodeHeader(&header.PacketType, &header.Size));
				THEN("Size reflects the 5 data bytes")
				{
					CHECK(header.PacketType == ByteSwappedPacketType);
					CHECK(header.Size == 5);
				}
			}
		}

		GIVEN("A Packet with two UInt32 values (12 and 13) written via <<")
		{
			PacketHeader header = {};
			Packet packet5(PacketType);
			packet5 << UInt32(12) << UInt32(13);

			WHEN("Header is encoded then decoded, and values are read back")
			{
				CHECK(packet5.EncodeHeader());
				CHECK(packet5.DecodeHeader(&header.PacketType, &header.Size));
				THEN("Size and read-back values are correct")
				{
					CHECK(header.PacketType == ByteSwappedPacketType);
					CHECK(header.Size == 2 * sizeof(Int32));
					UInt32 a, b;
					packet5 >> a >> b;
					CHECK(a == 12);
					CHECK(b == 13);
				}
			}
		}
	}
} // namespace CCT_ANONYMOUS_NAMESPACE
