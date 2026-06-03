#pragma once

#include <concepts>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace cct
{

	// Thread-safe single-consumer queue.
	// - Push() is safe from any thread.
	// - DrainTo() and WaitAndDrainTo() must be called from the consumer thread only.
	template<typename T>
	class ThreadQueue
	{
	public:
		// Push a value from any thread.
		void Push(T value);

		// Non-blocking: swap internal buffer and invoke fn for each item.
		// Returns the number of items processed.
		template<typename F>
			requires std::invocable<F, T>
		std::size_t DrainTo(F&& fn);

		// Blocking: sleep until items are available or stop is requested, then drain.
		// Drains any remaining items before returning after stop.
		template<typename F>
			requires std::invocable<F, T>
		void WaitAndDrainTo(F&& fn, std::stop_token st);

		bool IsEmpty() const;
		std::size_t Size() const;

	private:
		mutable std::mutex m_mutex;
		std::condition_variable m_cv;
		std::vector<T> m_queue;
		std::vector<T> m_drainBuf;
	};

} // namespace cct

#include "ThreadQueue.inl"
