//
// Created by arthur on 18/08/2026.
//

#include "Concerto/Core/ThreadAffinity/ThreadAffinity.hpp"

#include <atomic>
#include <string>

namespace cct
{
	namespace
	{
		std::atomic<bool> g_hasDomainThread{false};
		std::thread::id g_domainThread{};

		std::string& CurrentName()
		{
			thread_local std::string name = "unnamed";
			return name;
		}
	} // namespace

	void ThreadAffinity::DeclareDomainThread()
	{
		g_domainThread = std::this_thread::get_id();
		g_hasDomainThread.store(true, std::memory_order_release);
	}

	void ThreadAffinity::ClearDomainThread()
	{
		g_hasDomainThread.store(false, std::memory_order_release);
	}

	bool ThreadAffinity::HasDomainThread()
	{
		return g_hasDomainThread.load(std::memory_order_acquire);
	}

	bool ThreadAffinity::IsDomainThread()
	{
		if (!g_hasDomainThread.load(std::memory_order_acquire))
			return true;
		return std::this_thread::get_id() == g_domainThread;
	}

	void ThreadAffinity::SetCurrentName(std::string_view name)
	{
		CurrentName().assign(name);
	}

	std::string_view ThreadAffinity::GetCurrentName()
	{
		return CurrentName();
	}
} // namespace cct
