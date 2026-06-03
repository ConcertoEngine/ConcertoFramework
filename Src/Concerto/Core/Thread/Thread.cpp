#include "Concerto/Core/Thread/Thread.hpp"

#ifdef CCT_PLATFORM_WINDOWS
#include <windows.h>
#elif defined(CCT_PLATFORM_POSIX)
#include <pthread.h>
#endif

namespace cct
{

	static constexpr std::size_t kLinuxMaxThreadNameLen = 15;
	static constexpr std::size_t kMacosMaxThreadNameLen = 63;

	void Thread::SetName(std::string_view name)
	{
		m_name = std::string(name);
		if (m_thread.joinable())
			ApplyPlatformName(m_thread, name);
	}

	void Thread::RequestStop()
	{
		m_thread.request_stop();
	}

	void Thread::Join()
	{
		if (m_thread.joinable())
			m_thread.join();
	}

	bool Thread::IsJoinable() const
	{
		return m_thread.joinable();
	}

	std::string_view Thread::GetName() const
	{
		return m_name;
	}

	void Thread::SetCurrentThreadName(std::string_view name)
	{
#ifdef CCT_PLATFORM_WINDOWS
		std::wstring wname(name.begin(), name.end());
		SetThreadDescription(GetCurrentThread(), wname.c_str());
#elif defined(CCT_PLATFORM_LINUX)
		char buf[kLinuxMaxThreadNameLen + 1] = {};
		name.copy(buf, std::min(name.size(), kLinuxMaxThreadNameLen));
		pthread_setname_np(pthread_self(), buf);
#elif defined(CCT_PLATFORM_MACOS)
		char buf[kMacosMaxThreadNameLen + 1] = {};
		name.copy(buf, std::min(name.size(), kMacosMaxThreadNameLen));
		pthread_setname_np(buf);
#else
		(void)name;
#endif
	}

	void Thread::ApplyPlatformName(std::jthread& t, std::string_view name)
	{
#ifdef CCT_PLATFORM_WINDOWS
		std::wstring wname(name.begin(), name.end());
		SetThreadDescription(t.native_handle(), wname.c_str());
#elif defined(CCT_PLATFORM_LINUX)
		char buf[kLinuxMaxThreadNameLen + 1] = {};
		name.copy(buf, std::min(name.size(), kLinuxMaxThreadNameLen));
		pthread_setname_np(t.native_handle(), buf);
#else
		(void)t;
		(void)name;
#endif
	}

} // namespace cct
