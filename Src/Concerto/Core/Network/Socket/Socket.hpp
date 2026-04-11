//
// Created by arthur on 25/05/2023.
//

#ifndef CONCERTO_CORE_NETWORK_SOCKET_HPP
#define CONCERTO_CORE_NETWORK_SOCKET_HPP

#include <cstddef>

#include "Concerto/Core/Network/Socket/SocketHandle.hpp"
#include "Concerto/Core/Network/IpAddress/IpAddress.hpp"

namespace cct
{
	class Buffer;
}

namespace cct::net
{
	class CCT_CORE_PUBLIC_API Socket final
	{
	 public:
		Socket() = delete;
		~Socket();
		Socket(SocketType socketType, IpProtocol ipProtocol);
		Socket(const Socket&) = delete;
		Socket(Socket&&) noexcept;

		Socket& operator=(const Socket&) = delete;
		Socket& operator=(Socket&&) noexcept;

		void Close();

		[[nodiscard]] SocketType GetType() const;
		[[nodiscard]] SocketError GetLastError() const;
		[[nodiscard]] std::size_t GetAvailableBytes() const;

		void SetBlocking(bool blocking);

		void Listen(const IpAddress& address, int backlog = 5);
		bool Bind(const IpAddress& address);
		bool Accept(Socket& socket);
		void Connect(IpAddress address);
		std::size_t Receive(Buffer& buffer);
		std::size_t Receive(void* buffer, std::size_t size);

		std::size_t Send(const Buffer& buffer);
		std::size_t Send(const void* buffer, std::size_t size);

		static bool Initialize();
		static bool UnInitialize();
	 protected:
		SocketHandle _handle{};
		SocketType _type;
		SocketError _lastError;
		IpProtocol _ipProtocol;
		bool _blocking;
	};
}

#endif //CONCERTO_CORE_NETWORK_SOCKET_HPP
