#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <thread>

#include "Concerto/Core/Defines.hpp"

namespace cct
{

	class CCT_CORE_PUBLIC_API Thread
	{
	public:
		Thread() = default;
		~Thread() = default;

		Thread(const Thread&) = delete;
		Thread& operator=(const Thread&) = delete;
		Thread(Thread&&) = default;
		Thread& operator=(Thread&&) = default;

		// Start the thread. fn may optionally accept std::stop_token as its first argument.
		template<typename F>
		void Start(std::string_view name, F&& fn);

		// Rename a running thread (no-op on macOS, use Start() which names from within).
		void SetName(std::string_view name);

		void RequestStop();
		void Join();
		bool IsJoinable() const;
		std::string_view GetName() const;

	private:
		// Called from within the new thread at start - works on all platforms.
		static void SetCurrentThreadName(std::string_view name);
		// Called from outside via handle - works on Windows and Linux only.
		static void ApplyPlatformName(std::jthread& t, std::string_view name);

		std::jthread m_thread;
		std::string m_name;
	};

} // namespace cct

#include "Thread.inl"
