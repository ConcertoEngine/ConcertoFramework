namespace cct
{

	template<typename F>
	void Thread::Start(std::string_view name, F&& fn)
	{
		m_name = std::string(name);
		m_thread = std::jthread(
			[fn = std::forward<F>(fn), nameStr = m_name](std::stop_token st) mutable
			{
				Thread::SetCurrentThreadName(nameStr);
				if constexpr (std::invocable<std::decay_t<decltype(fn)>, std::stop_token>)
					fn(std::move(st));
				else
					fn();
			});
	}

} // namespace cct
