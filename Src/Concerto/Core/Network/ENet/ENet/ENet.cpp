//
// Created by arthur on 12/08/2023.
//

#ifdef CCT_ENABLE_ENET

#include "Concerto/Core/Network/ENet/ENet/ENet.hpp"

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Logger/Logger.hpp"
#include <enet6/enet.h>

namespace cct::net
{
	void ENet::Initialize()
	{
		const auto res = enet_initialize();
		if (res == 0)
			return;
		CCT_ASSERT_FALSE("An error occurred while initializing Enet: {}", res);
	}

	void ENet::Deinitialize()
	{
		enet_deinitialize();
	}
} // namespace cct::net

#endif // CCT_ENABLE_ENET