//
// Created by arthur on 25/05/2023.
//

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Network/Socket/Socket.hpp"
#include "Concerto/Core/Buffer/Buffer.hpp"
#ifdef CCT_PLATFORM_WINDOWS
#include "Concerto/Core/Network/Socket/Private/WinSocketImpl.hpp"
#endif// CCT_PLATFORM_WINDOWS
#ifdef CCT_PLATFORM_POSIX
#include "Concerto/Core/Network/Socket/Private/PosixSocketImpl.hpp"
#endif//CCT_PLATFORM_POSIX

namespace cct::net
{
	Socket::Socket(SocketType socketType, IpProtocol ipProtocol) :
		_handle(SocketImpl::InvalidSocket), 
		_type(socketType),
		_lastError(SocketError::NoError),
		_ipProtocol(ipProtocol),
		_blocking(false)
	{
	}

  Socket::Socket(Socket&& other) noexcept :
		_handle(other._handle),
		_type(other._type),
		_lastError(other._lastError),
		_ipProtocol(other._ipProtocol),
		_blocking(other._blocking)
	{
		_handle = other._handle;
		_lastError = other._lastError;
		other._handle = SocketImpl::InvalidSocket;
		other._lastError = SocketError::NoError;
	}

	Socket::~Socket()
	{
		Close();
	}

    Socket& Socket::operator=(Socket&& other) noexcept
    {
		_handle = other._handle;
		_lastError = other._lastError;
		other._handle = SocketImpl::InvalidSocket;
		other._lastError = SocketError::NoError;
		return *this;
    }

	void Socket::Close()
	{
		if (_handle == SocketImpl::InvalidSocket)
			return;
		SocketImpl::Close(_handle, &_lastError);
		_handle = SocketImpl::InvalidSocket;
	}

	SocketType Socket::GetType() const
	{
		return _type;
	}

	SocketError Socket::GetLastError() const
	{
		return _lastError;
	}

	std::size_t Socket::GetAvailableBytes() const
	{
		if (_handle == SocketImpl::InvalidSocket)
			return 0;
		SocketError error = {};
		const std::size_t availableBytes =  SocketImpl::GetAvailableBytes(_handle, &error);
		if (error != SocketError::NoError)
			CCT_ASSERT_FALSE("ConcertoCore: GetAvailableBytes returned error: {}", static_cast<int>(error));
		return availableBytes;
	}

	void Socket::SetBlocking(bool blocking)
	{
		if (_handle == SocketImpl::InvalidSocket)
			return;
		SocketImpl::SetBlocking(_handle, blocking, &_lastError);
	}

	void Socket::Listen(const IpAddress& address, int backlog)
	{
		const SocketHandle handle = SocketImpl::Create(_type, _ipProtocol, &_lastError);
		if (handle == SocketImpl::InvalidSocket)
			return;
		if (!SocketImpl::Listen(handle, address, backlog, &_lastError))
			return;
		_handle = handle;
	}

	bool Socket::Bind(const IpAddress& address)
	{
		const SocketHandle handle = SocketImpl::Create(_type, _ipProtocol, &_lastError);
		if (handle == SocketImpl::InvalidSocket)
			return false;
		if (!SocketImpl::Bind(handle, address, &_lastError))
			return false;
		_handle = handle;
		return true;
	}

	bool Socket::Accept(Socket& socket)
	{
		CCT_ASSERT(_handle != SocketImpl::InvalidSocket, "Invalid socket handle");
		const SocketHandle handle = SocketImpl::Accept(_handle, nullptr, &_lastError);
		if (handle == SocketImpl::InvalidSocket)
			return false;
		socket.Close();
		socket._handle = handle;
		return true;
	}

	void Socket::Connect(IpAddress address)
	{
		Close();
		const SocketHandle handle = SocketImpl::Create(_type, _ipProtocol, &_lastError);
		if (handle == SocketImpl::InvalidSocket)
			return;
		if (!SocketImpl::Connect(handle, address, &_lastError))
			return;
		_handle = handle;
	}

	std::size_t Socket::Receive(Buffer& buffer)
	{
		return Receive(buffer.GetRawData(), buffer.Size());
	}

	std::size_t Socket::Receive(void* buffer, std::size_t size)
	{
		CCT_ASSERT(_handle != SocketImpl::InvalidSocket, "Invalid socket handle");
		std::size_t received = 0;
		SocketImpl::Receive(_handle, buffer, size, &received, &_lastError);
		return received;
	}

	std::size_t Socket::Send(const Buffer& buffer)
	{
		return Send(buffer.GetRawData(), buffer.Size());
	}

	std::size_t Socket::Send(const void* buffer, std::size_t size)
	{
		CCT_ASSERT(_handle != SocketImpl::InvalidSocket, "Invalid socket handle");
		std::size_t sent = 0;
		SocketImpl::Send(_handle, buffer, size, &sent, &_lastError);
		return sent;
	}

	bool Socket::Initialize()
	{
		return SocketImpl::Initialize();
	}

	bool Socket::UnInitialize()
	{
		return SocketImpl::UnInitialize();
	}
}