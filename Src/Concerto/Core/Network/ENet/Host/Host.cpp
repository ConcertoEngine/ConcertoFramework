//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#include "Concerto/Core/Network/ENet/Host/Host.hpp"

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Logger/Logger.hpp"
#include "Concerto/Core/Network/ENet/Address/Address.hpp"

namespace cct::net
{
	::ENetHost* ToENetHost(ENetHost::Handle handle)
	{
		return static_cast<::ENetHost*>(handle);
	}

	ENetHost::ENetHost(IpAddress* address, std::size_t maxConnections,
					   std::size_t maxChannels, UInt32 maxIncomingBandwidth, UInt32 maxOutgoingBandwidth,
					   IpProtocol protocol) :
		_enetHost(nullptr),
		_maxConnections(maxConnections),
		_maxChannels(maxChannels),
		_maxIncomingBandwidth(maxIncomingBandwidth),
		_maxOutgoingBandwidth(maxOutgoingBandwidth),
		_protocol(protocol)
	{
		[[maybe_unused]] const bool ret = CreateHost(address);
		CCT_ASSERT(ret, "An error occurred while trying to create an ENetHost");
	}

	ENetHost::~ENetHost()
	{
		enet_host_destroy(ToENetHost(_enetHost));
	}

	Int32 ENetHost::PollEvent(ENetEvent* event, UInt32 timeout)
	{
		CCT_ASSERT(_enetHost != nullptr, "Invalid host");
		::ENetEvent enetEvent;
		const Int32 ret = enet_host_service(ToENetHost(_enetHost), &enetEvent, timeout);
		event->timedOut = false;
		if (ret <= 0)
		{
			event->eventType = ENetEvent::Type::None;
			event->peer = nullptr;
			return ret;
		}

		switch (enetEvent.type)
		{
			case ENET_EVENT_TYPE_NONE:
			{
				event->eventType = ENetEvent::Type::None;
				break;
			}
			case ENET_EVENT_TYPE_CONNECT:
			{
				event->eventType = ENetEvent::Type::Connect;
				event->peer = std::make_unique<ENetPeer>(static_cast<void*>(enetEvent.peer));
				event->channelId = enetEvent.channelID;
				event->data = enetEvent.data;
				event->packet = nullptr;
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
			{
				event->eventType = ENetEvent::Type::Disconnect;
				event->timedOut = (enetEvent.type == ENET_EVENT_TYPE_DISCONNECT_TIMEOUT);
				event->peer = std::make_unique<ENetPeer>(static_cast<void*>(enetEvent.peer));
				event->channelId = enetEvent.channelID;
				event->data = enetEvent.data;
				event->packet = nullptr;
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				event->eventType = ENetEvent::Type::Receive;
				event->peer = std::make_unique<ENetPeer>(static_cast<void*>(enetEvent.peer));
				event->channelId = enetEvent.channelID;
				event->data = enetEvent.data;
				event->packet = std::make_unique<ENetPacket>();
				event->packet->Write(enetEvent.packet->data, enetEvent.packet->dataLength);
				event->packet->SetCursorPos(0);
				break;
			}
		}
		return ret;
	}

	bool ENetHost::SendPacket(const void* data, std::size_t size, ENetPeer* peer, UInt8 channel, ENetPacket::Flag flags)
	{
		CCT_ASSERT(peer != nullptr, "Invalid peer");
		return peer->SendPacket(data, size, channel, flags);
	}

	bool ENetHost::SendPacket(const ENetPacket& packet, ENetPeer* peer, UInt8 channel, ENetPacket::Flag flags)
	{
		return SendPacket(packet.GetData(), packet.GetSize(), peer, channel, flags);
	}

	void ENetHost::Broadcast(const void* data, std::size_t size, UInt8 channel, ENetPacket::Flag flags)
	{
		enet_uint32 enetFlags = ENET_PACKET_FLAG_RELIABLE;
		if (flags == ENetPacket::Flag::Unsequenced)
			enetFlags = ENET_PACKET_FLAG_UNSEQUENCED;
		else if (flags == ENetPacket::Flag::UnreliableFragment)
			enetFlags = ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT;
		::ENetPacket* pkt = enet_packet_create(data, size, enetFlags);
		if (pkt)
			enet_host_broadcast(ToENetHost(_enetHost), channel, pkt);
	}

	void ENetHost::Broadcast(const ENetPacket& packet, UInt8 channel, ENetPacket::Flag flags)
	{
		Broadcast(packet.GetData(), static_cast<std::size_t>(packet.GetSize()), channel, flags);
	}

	void ENetHost::Flush()
	{
		enet_host_flush(ToENetHost(_enetHost));
	}

	bool ENetHost::CreateHost(IpAddress* address)
	{
		const ::ENetAddressType type = ToENetAddressType(_protocol);
		if (address == nullptr)
		{
			_enetHost = enet_host_create(type, nullptr, _maxConnections, _maxChannels, _maxIncomingBandwidth, _maxOutgoingBandwidth);
			return _enetHost != nullptr;
		}

		::ENetAddress enetAddress = {};
		if (!ToENetAddress(*address, type, enetAddress))
		{
			CCT_ASSERT_FALSE("Address '{}' cannot be bound by this host", address->ToString());
			return false;
		}

		_enetHost = enet_host_create(type, &enetAddress, _maxConnections, _maxChannels, _maxIncomingBandwidth, _maxOutgoingBandwidth);
		return _enetHost != nullptr;
	}
} // namespace cct::net

#endif // CCT_ENABLE_ENET