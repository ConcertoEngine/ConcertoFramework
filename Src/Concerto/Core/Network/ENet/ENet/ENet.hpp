//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#ifndef CONCERTO_CORE_NETWORK_ENET_HPP
#define CONCERTO_CORE_NETWORK_ENET_HPP

#include "Concerto/Core/Types/Types.hpp"

namespace cct::net
{
	class CCT_CORE_PUBLIC_API ENet
	{
	 public:
		static void Initialize();
		static void Deinitialize();
	};
}// namespace cct::net

#endif // CONCERTO_CORE_NETWORK_ENET_HPP

#endif // CCT_ENABLE_ENET