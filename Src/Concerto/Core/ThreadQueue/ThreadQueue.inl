namespace cct
{

	template<typename T>
	void ThreadQueue<T>::Push(T value)
	{
		{
			std::lock_guard lock(m_mutex);
			m_queue.push_back(std::move(value));
		}
		m_cv.notify_one();
	}

	template<typename T>
	template<typename F>
		requires std::invocable<F, T>
	std::size_t ThreadQueue<T>::DrainTo(F&& fn)
	{
		m_drainBuf.clear();
		{
			std::lock_guard lock(m_mutex);
			std::swap(m_queue, m_drainBuf);
		}
		for (auto& item : m_drainBuf)
			fn(std::move(item));
		return m_drainBuf.size();
	}

	template<typename T>
	template<typename F>
		requires std::invocable<F, T>
	void ThreadQueue<T>::WaitAndDrainTo(F&& fn, std::stop_token st)
	{
		// When stop is requested, wake the CV so the wait loop can exit.
		std::stop_callback wakeOnStop(st, [this]
									  { m_cv.notify_all(); });

		while (!st.stop_requested())
		{
			m_drainBuf.clear();
			{
				std::unique_lock lock(m_mutex);
				m_cv.wait(lock, [&]
						  { return !m_queue.empty() || st.stop_requested(); });
				std::swap(m_queue, m_drainBuf);
			}
			for (auto& item : m_drainBuf)
				fn(std::move(item));
		}
		// Drain any items that arrived between the last swap and the stop.
		DrainTo(std::forward<F>(fn));
	}

	template<typename T>
	bool ThreadQueue<T>::IsEmpty() const
	{
		std::lock_guard lock(m_mutex);
		return m_queue.empty();
	}

	template<typename T>
	std::size_t ThreadQueue<T>::Size() const
	{
		std::lock_guard lock(m_mutex);
		return m_queue.size();
	}

} // namespace cct
