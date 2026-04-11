//
// Created by arthur on 25/05/2023.
//

#include "Concerto/Core/Types/Types.hpp"
#ifdef CCT_PLATFORM_WINDOWS

#include <string>
#include <cassert>

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Logger/Logger.hpp"
#include "WinSocketImpl.hpp"
#include "Concerto/Core/Network/IpAddress/Private/WinIpAddressImpl.hpp"

namespace cct::net
{
	SocketHandle const SocketImpl::InvalidSocket = INVALID_SOCKET;

	SocketHandle SocketImpl::Accept(SocketHandle socket, IpAddress* address, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		sockaddr addr = {};
		int addrSize = sizeof(sockaddr);
		const SocketHandle newSocket = accept(socket, &addr, &addrSize);
		if (newSocket == InvalidSocket)
		{
			if (error != nullptr)
				*error = GetSocketError(WSAGetLastError());
		}
		else
		{
			if (address != nullptr)
				*address = IpAddressImpl::FromSockAddr(&addr);
		}
		return newSocket;
	}

	bool SocketImpl::Connect(SocketHandle socket, const IpAddress& address, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		const sockaddr_in addr = IpAddressImpl::ToSockAddr(address);

		if (connect(socket, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr)) == SOCKET_ERROR)
		{
			if (error != nullptr)
				*error = GetSocketError(WSAGetLastError());
			return false;
		}
		return true;
	}

	SocketHandle SocketImpl::Create(SocketType socketType, IpProtocol protocol, SocketError* error)
	{
		if (error != nullptr)
			*error = SocketError::NoError;
		const SocketHandle socket = ::socket(protocol == IpProtocol::Ipv4 ? AF_INET : AF_INET6,
			socketType == SocketType::Tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
		if (socket == InvalidSocket)
		{
			if (error != nullptr)
				*error = GetSocketError(WSAGetLastError());
		}
		return socket;
	}

	bool SocketImpl::Bind(SocketHandle socket, const IpAddress& address, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		const sockaddr_in addr = IpAddressImpl::ToSockAddr(address);
		if (bind(socket, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr)) == SOCKET_ERROR)
		{
			if (error != nullptr)
				*error = GetSocketError(WSAGetLastError());
			return false;
		}
		return true;
	}

	bool SocketImpl::Listen(SocketHandle socket, const IpAddress& address, int backlog, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		if (!Bind(socket, address, error))
			return false;
		if (listen(socket, backlog) == SOCKET_ERROR)
		{
			if (error != nullptr)
				*error = GetSocketError(WSAGetLastError());
			return false;
		}
		return true;
	}

	bool SocketImpl::Close(SocketHandle socket, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		if (closesocket(socket) == SOCKET_ERROR)
		{
			if (error != nullptr)
				*error = GetSocketError(WSAGetLastError());
			return false;
		}
		return true;
	}

	bool SocketImpl::SetBlocking(SocketHandle socket, bool blocking, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		u_long mode = blocking ? 0 : 1;
		if (ioctlsocket(socket, FIONBIO, &mode) == SOCKET_ERROR)
		{
			if (error != nullptr)
				*error = GetSocketError(WSAGetLastError());
			return false;
		}
		return true;
	}

	bool SocketImpl::Receive(SocketHandle socket,
		void* buffer,
		std::size_t size,
		std::size_t* received,
		SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		const int result = recv(socket, static_cast<char*>(buffer), static_cast<Int32>(size), 0);
		if (result == SOCKET_ERROR)
		{
			if (error != nullptr)
				*error = GetSocketError(WSAGetLastError());
			return false;
		}
		if (received != nullptr)
			*received = result;
		return true;
	}

	bool SocketImpl::Send(SocketHandle socket, const void* buffer, std::size_t size, std::size_t* sent, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		const int result = send(socket, static_cast<const char*>(buffer), static_cast<Int32>(size), 0);
		if (result == SOCKET_ERROR)
		{
			if (error != nullptr)
				*error = GetSocketError(WSAGetLastError());
			return false;
		}
		if (sent != nullptr)
			*sent = result;
		return true;
	}

	bool SocketImpl::Initialize()
	{
		WSADATA wsaData;
		return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
	}

	bool SocketImpl::UnInitialize()
	{
		return WSACleanup() == 0;
	}

	SocketError SocketImpl::GetSocketError(int error)
	{
		switch (error)
		{
			case 0:
				return SocketError::NoError;
			case WSAECONNREFUSED:
				return SocketError::ConnectionRefused;
			case WSAECONNRESET:
			case WSAECONNABORTED:
			case WSAENOTCONN:
			case WSAESHUTDOWN:
				return SocketError::ConnectionClosed;
			case WSAEADDRINUSE:
			case WSAEADDRNOTAVAIL:
				return SocketError::AddressNotAvailable;
			case WSAEACCES:
			case WSAEFAULT:
			case WSAEBADF:
			case WSAENOTSOCK:
			case WSAEALREADY:
			case WSAEISCONN:
				return SocketError::InternError;
			case WSANOTINITIALISED:
				return SocketError::NotInitialized;
			case WSAETIMEDOUT:
				return SocketError::TimedOut;
			default:
			{
				Logger::Warning("Unknown socket error: {}", error);
				return SocketError::Unknown;
			}
		}
	}

	int SocketImpl::GetSocketState(SocketHandle socket, SocketError* error)
	{
		int status;
		int statusSize = sizeof(status);
		if (getsockopt(socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&status), &statusSize) == SOCKET_ERROR)
		{
			if (error)
				*error = GetSocketError(WSAGetLastError());
			closesocket(socket);
			WSACleanup();
			return 1;
		}
		return 0;
	}

	std::size_t SocketImpl::GetAvailableBytes(SocketHandle handle, SocketError* error)
	{
		CCT_ASSERT(handle != SocketImpl::InvalidSocket, "Invalid socket handle");
		u_long available = 0;
		if (ioctlsocket(handle, FIONREAD, &available) == SOCKET_ERROR)
		{
			if (error)
				*error = GetSocketError(WSAGetLastError());
			return 0;
		}
		return static_cast<std::size_t>(available);
	}
}

#endif