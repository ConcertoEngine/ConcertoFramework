//
// Created by arthur on 18/08/2026.
//

#ifndef CONCERTO_CORE_THREAD_AFFINITY_HPP
#define CONCERTO_CORE_THREAD_AFFINITY_HPP

#include <string_view>
#include <thread>

#include "Concerto/Core/Defines.hpp"

namespace cct
{
	class CCT_CORE_PUBLIC_API ThreadAffinity
	{
	public:
		static void DeclareDomainThread();
		static void ClearDomainThread();
		static bool HasDomainThread();
		static bool IsDomainThread();

		static void SetCurrentName(std::string_view name);
		static std::string_view GetCurrentName();
	};
} // namespace cct

#ifdef CCT_ENABLE_ASSERTS
	#include <Concerto/Core/Assert.hpp>
	#define CCT_ASSERT_DOMAIN_THREAD()                                                        \
		CCT_ASSERT(cct::ThreadAffinity::IsDomainThread(),                                     \
				   "reflected state mutated from thread '{}', which is not the domain thread", \
				   cct::ThreadAffinity::GetCurrentName())
#else
	#define CCT_ASSERT_DOMAIN_THREAD() ((void) 0)
#endif

#endif // CONCERTO_CORE_THREAD_AFFINITY_HPP
