//
// Created by arthur on 25/05/2023.
//

#ifndef CONCERTO_CORE_NETWORK_SOCKETHANDLE_HPP
#define CONCERTO_CORE_NETWORK_SOCKETHANDLE_HPP

#include "Concerto/Core/Types/Types.hpp"
#ifdef CCT_PLATFORM_WINDOWS
#include <basetsd.h>
#endif
namespace cct::net
{
#ifdef CCT_PLATFORM_WINDOWS
	using SocketHandle = UINT_PTR;
#else
	using SocketHandle = int;
#endif

	enum class SocketType
	{
		Tcp,
		Udp
	};

	enum class SocketError
	{
		NoError, // No error
		ConnectionRefused, // Connection with remote host was refused
		ConnectionClosed, // Connection with remote host was closed
		AddressNotAvailable, // Address is not available
		InternError, // Internal error
		NotInitialized, // Socket is not initialized
		TimedOut, // Connection timed out
		NetworkError, // Network error
		UnreachableHost, // Unreachable host
		Unknown // Unknown error
	};

}
#endif //CONCERTO_CORE_NETWORK_SOCKETHANDLE_HPP
