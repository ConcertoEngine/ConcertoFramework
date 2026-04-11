//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#include <enet/enet.h>
#include "Concerto/Core/Network/ENet/Host/Host.hpp"
#include "Concerto/Core/Logger/Logger.hpp"
#include "Concerto/Core/Assert.hpp"

namespace cct::net
{
	::ENetHost* ToENetHost(ENetHost::Handle handle)
	{
		return static_cast<::ENetHost*>(handle);
	}

	ENetHost::ENetHost(IpAddress* address, std::size_t maxConnections,
		std::size_t maxChannels, UInt32 maxIncomingBandwidth, UInt32 maxOutgoingBandwidth) :
		_enetHost(nullptr),
		_maxConnections(maxConnections),
		_maxChannels(maxChannels),
		_maxIncomingBandwidth(maxIncomingBandwidth),
		_maxOutgoingBandwidth(maxOutgoingBandwidth)
	{
		[[maybe_unused]]const bool ret = CreateHost(address);
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
			{
				event->eventType = ENetEvent::Type::Disconnect;
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

	void ENetHost::Flush()
	{
		enet_host_flush(ToENetHost(_enetHost));
	}

	bool ENetHost::CreateHost(IpAddress* address)
	{
		if (address == nullptr)
		{
			_enetHost = enet_host_create(nullptr, _maxConnections, _maxChannels, _maxIncomingBandwidth, _maxOutgoingBandwidth);
			return _enetHost != nullptr;
		}
		
		::ENetAddress enetAddress = {
			.host = address->ToUInt32(),
			.port = address->GetPort()
		};
		_enetHost = enet_host_create(&enetAddress, _maxConnections, _maxChannels, _maxIncomingBandwidth, _maxOutgoingBandwidth);
		return _enetHost != nullptr;
	}
}

#endif // CCT_ENABLE_ENET