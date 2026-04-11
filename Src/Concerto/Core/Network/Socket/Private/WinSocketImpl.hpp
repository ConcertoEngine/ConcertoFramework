//
// Created by arthur on 25/05/2023.
//

#ifndef CONCERTO_CORE_NETWORK_SOCKET_WINSOCKETIMPL_HPP
#define CONCERTO_CORE_NETWORK_SOCKET_WINSOCKETIMPL_HPP

#include "Concerto/Core/Types/Types.hpp"
#include "Concerto/Core/Network/Socket/SocketHandle.hpp"
#include "Concerto/Core/Network/IpAddress/IpAddress.hpp"

namespace cct::net
{
	class SocketImpl
	{
	 public:
		SocketImpl() = delete;
		~SocketImpl() = delete;

		static SocketHandle Accept(SocketHandle socket, IpAddress* address, SocketError* error = nullptr);
		static bool Connect(SocketHandle socket, const IpAddress& address, SocketError* error = nullptr);
		static SocketHandle Create(SocketType socketType, IpProtocol protocol, SocketError* error);
		static bool Bind(SocketHandle socket, const IpAddress& address, SocketError* error = nullptr);
		static bool Listen(SocketHandle socket, const IpAddress& address, int backlog, SocketError* error = nullptr);
		static bool Close(SocketHandle socket, SocketError* error = nullptr);
		static bool SetBlocking(SocketHandle socket, bool blocking, SocketError* error = nullptr);
		static bool Receive(SocketHandle socket, void* buffer, std::size_t size, std::size_t* received, SocketError* error = nullptr);
		static bool Send(SocketHandle socket, const void* buffer, std::size_t size, std::size_t* sent, SocketError* error = nullptr);
		static int GetSocketState(SocketHandle socket, SocketError* error = nullptr);
		static bool Initialize();
		static bool UnInitialize();
		static SocketError GetSocketError(int error);
		static std::size_t GetAvailableBytes(SocketHandle handle, SocketError* error = nullptr);
		static const SocketHandle InvalidSocket;
	};
}// namespace cct::net
#endif //CONCERTO_CORE_NETWORK_SOCKET_WINSOCKETIMPL_HPP
