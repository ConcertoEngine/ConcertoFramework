//
// Created by arthur on 28/05/2023.
//
#include "Concerto/Core/Types/Types.hpp"
#ifdef CCT_PLATFORM_POSIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Network/IpAddress/Private/PosixIpAddressImpl.hpp"
#include "Concerto/Core/Network/Socket/Private/PosixSocketImpl.hpp"

namespace cct::net
{
	SocketHandle constexpr SocketImpl::InvalidSocket = -1;

	SocketHandle SocketImpl::Accept(SocketHandle socket, IpAddress* address, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		sockaddr_in addr;
		socklen_t len = sizeof(addr);
		SocketHandle handle = accept(socket, reinterpret_cast<sockaddr*>(&addr), &len);
		if (handle == InvalidSocket)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
			return InvalidSocket;
		}
		if (address != nullptr)
			*address = IpAddressImpl::FromSockAddr(reinterpret_cast<sockaddr*>(&addr));
		return handle;
	}

	bool SocketImpl::Connect(SocketHandle socket, const IpAddress& address, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		sockaddr_in addr = IpAddressImpl::ToSockAddr(address);
		if (connect(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr)) == -1)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
			return false;
		}
		return true;
	}

	SocketHandle SocketImpl::Create(SocketType socketType, IpProtocol protocol, SocketError* error)
	{
		SocketHandle handle = socket(protocol == IpProtocol::Ipv4 ? AF_INET : AF_INET6,
									 socketType == SocketType::Tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
		if (handle == InvalidSocket)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
			return InvalidSocket;
		}
		return handle;
	}

	bool SocketImpl::Bind(SocketHandle socket, const IpAddress& address, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		sockaddr_in addr = IpAddressImpl::ToSockAddr(address);
		if (bind(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr)) == -1)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
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
		if (listen(socket, backlog) == -1)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
			return false;
		}
		return true;
	}

	bool SocketImpl::Close(SocketHandle socket, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		if (close(socket) == -1)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
			return false;
		}
		return true;
	}

	bool SocketImpl::SetBlocking(SocketHandle socket, bool blocking, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		int flags = fcntl(socket, F_GETFL, 0);
		if (flags == -1)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
			return false;
		}
		if (blocking)
			flags &= ~O_NONBLOCK;
		else
			flags |= O_NONBLOCK;
		if (fcntl(socket, F_SETFL, flags) == -1)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
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
		auto ret = recv(socket, buffer, size, 0);
		if (ret == -1)
		{
			if (ret == EWOULDBLOCK || ret == EAGAIN)
			{
				if (received != nullptr)
					*received = 0;
				return false;
			}
			if (error != nullptr)
				*error = GetSocketError(errno);
			return false;
		}
		else if (ret == 0)
		{
			if (error != nullptr)
				*error = SocketError::ConnectionClosed;
			return false;
		}
		if (received != nullptr)
			*received = static_cast<std::size_t>(ret);
		return true;
	}

	bool SocketImpl::Send(SocketHandle socket,
						  const void* buffer,
						  std::size_t size,
						  std::size_t* sent,
						  SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		auto ret = send(socket, buffer, size, 0);
		if (ret == -1)
		{
			int err = errno;
			if (err == EWOULDBLOCK || err == EAGAIN)
			{
				if (sent != nullptr)
					*sent = 0;
				return false;
			}
			if (error != nullptr)
				*error = GetSocketError(errno);
			return false;
		}
		if (sent != nullptr)
			*sent = static_cast<std::size_t>(ret);
		return true;
	}

	int SocketImpl::GetSocketState(SocketHandle socket, SocketError* error)
	{
		CCT_ASSERT(socket != SocketImpl::InvalidSocket, "Invalid socket handle");
		if (error != nullptr)
			*error = SocketError::NoError;
		int state = 0;
		socklen_t len = sizeof(state);
		if (getsockopt(socket, SOL_SOCKET, SO_ERROR, &state, &len) == -1)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
			return -1;
		}
		return state;
	}

	bool SocketImpl::Initialize()
	{
		return true;
	}

	bool SocketImpl::UnInitialize()
	{
		return true;
	}

	SocketError SocketImpl::GetSocketError(int error)
	{
		switch (error)
		{
		case EWOULDBLOCK:
		case EALREADY:
		case EBADF:
		case EFAULT:
		case EINVAL:
		case EISCONN:
		case ENOTSOCK:
		case EACCES:
			return SocketError::InternError;
		case EADDRINUSE:
		case EADDRNOTAVAIL:
			return SocketError::AddressNotAvailable;
		case ECONNREFUSED:
			return SocketError::ConnectionRefused;
		case ENETUNREACH:
		case ENETDOWN:
			return SocketError::NetworkError;
		case ETIMEDOUT:
			return SocketError::TimedOut;
		case EHOSTUNREACH:
			return SocketError::UnreachableHost;
		case ENOTCONN:
		case ESHUTDOWN:
			return SocketError::ConnectionClosed;
		default:
			return SocketError::Unknown;
		}
	}

	std::size_t SocketImpl::GetAvailableBytes(SocketHandle handle, SocketError* error)
	{
		CCT_ASSERT(handle != SocketImpl::InvalidSocket, "Invalid socket handle");
		std::size_t available = 0;
		if (ioctl(handle, FIONREAD, &available) == -1)
		{
			if (error != nullptr)
				*error = GetSocketError(errno);
		}
		return available;
	}
}// namespace cct::net
#endif// CCT_PLATFORM_LINUX