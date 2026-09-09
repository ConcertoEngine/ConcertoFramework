//
// Created by arthur on 27/02/2026.
//

#ifndef CONCERTO_CORE_SIGNAL_INL
#define CONCERTO_CORE_SIGNAL_INL

#include <algorithm>
#include <type_traits>

#include "Concerto/Core/Signal/Trackable.hpp"

namespace cct
{
	template<typename... Args>
	void Signal<Args...>::EnsureState() const
	{
		if (!m_state)
			m_state = std::make_shared<SlotList>();
	}

	template<typename... Args>
	Connection Signal<Args...>::MakeConnection(std::size_t id, std::shared_ptr<bool> connected)
	{
		std::weak_ptr<SlotList> weakState = m_state;
		std::weak_ptr<void> weakSignal = m_state;
		return Connection(
			[weakState, id, connected]()
			{
				if (!*connected)
					return;
				*connected = false;
				if (auto state = weakState.lock())
				{
					for (auto& s : state->slots)
					{
						if (s.id != id)
							continue;
						if (state->emitDepth > 0)
							s.active = false;
						else
							state->slots.erase(
								std::remove_if(state->slots.begin(), state->slots.end(),
											   [id](const Slot& e)
											   { return e.id == id; }),
								state->slots.end());
						return;
					}
				}
			},
			std::move(weakSignal), connected);
	}

	template<typename... Args>
	Connection Signal<Args...>::Connect(std::function<void(Args...)> slot)
	{
		EnsureState();
		const std::size_t id = m_state->nextId++;
		auto connected = std::make_shared<bool>(true);
		m_state->slots.push_back({id, std::move(slot), true, connected});
		return MakeConnection(id, connected);
	}

	template<typename... Args>
	template<typename T>
	Connection Signal<Args...>::Connect(T* obj, void (T::*method)(Args...))
	{
		if constexpr (std::is_base_of_v<Trackable, T>)
		{
			return Connect(*obj, obj, method);
		}
		else
		{
			return Connect([obj, method](Args... args)
						   { (obj->*method)(args...); });
		}
	}

	template<typename... Args>
	template<typename T>
	Connection Signal<Args...>::Connect(const T* obj, void (T::*method)(Args...) const)
	{
		if constexpr (std::is_base_of_v<Trackable, T>)
		{
			return Connect(*obj, std::function<void(Args...)>([obj, method](Args... args)
															  { (obj->*method)(args...); }));
		}
		else
		{
			return Connect([obj, method](Args... args)
						   { (obj->*method)(args...); });
		}
	}

	template<typename... Args>
	Connection Signal<Args...>::Connect(const Trackable& context, std::function<void(Args...)> slot)
	{
		EnsureState();
		std::weak_ptr<void> token = context.GetTrackToken();
		const std::size_t id = m_state->nextId++;
		auto connected = std::make_shared<bool>(true);
		Connection conn = MakeConnection(id, connected);
		std::function<void()> disconnectSelf = conn.m_disconnectFn;

		m_state->slots.push_back({id, [slot = std::move(slot), token, disconnectSelf](Args... args)
								  {
									  if (token.expired())
									  {
										  disconnectSelf();
										  return;
									  }
									  slot(args...);
								  },
								  true, connected});
		return conn;
	}

	template<typename... Args>
	template<typename T>
	Connection Signal<Args...>::Connect(const Trackable& context, T* obj, void (T::*method)(Args...))
	{
		return Connect(context, std::function<void(Args...)>([obj, method](Args... args)
															 { (obj->*method)(args...); }));
	}

	template<typename... Args>
	template<typename T>
	Connection Signal<Args...>::Connect(const Trackable& context, const T* obj, void (T::*method)(Args...) const)
	{
		return Connect(context, std::function<void(Args...)>([obj, method](Args... args)
															 { (obj->*method)(args...); }));
	}

	template<typename... Args>
	void Signal<Args...>::Disconnect(Connection& connection)
	{
		connection.Disconnect();
	}

	template<typename... Args>
	void Signal<Args...>::DisconnectAll()
	{
		if (!m_state)
			return;
		SlotList& state = *m_state;
		for (Slot& s : state.slots)
		{
			if (s.connected)
				*s.connected = false;
		}
		if (state.emitDepth > 0)
		{
			for (Slot& s : state.slots)
				s.active = false;
		}
		else
		{
			state.slots.clear();
		}
	}

	template<typename... Args>
	void Signal<Args...>::Emit(Args... args) const
	{
		if (!m_state)
			return;

		SlotList& state = *m_state;
		++state.emitDepth;
		for (const Slot& slot : state.slots)
		{
			if (slot.active)
				slot.fn(args...);
		}
		--state.emitDepth;

		if (state.emitDepth == 0)
		{
			state.slots.erase(
				std::remove_if(state.slots.begin(), state.slots.end(),
							   [](const Slot& s)
							   { return !s.active; }),
				state.slots.end());
		}
	}

	template<typename... Args>
	void Signal<Args...>::operator()(Args... args) const
	{
		Emit(args...);
	}

	template<typename... Args>
	std::size_t Signal<Args...>::GetConnectionCount() const
	{
		if (!m_state)
			return 0;
		return static_cast<std::size_t>(
			std::count_if(m_state->slots.begin(), m_state->slots.end(),
						  [](const Slot& s)
						  { return s.active; }));
	}

	template<typename... Args>
	bool Signal<Args...>::HasConnections() const
	{
		return GetConnectionCount() > 0;
	}
} // namespace cct

#endif // CONCERTO_CORE_SIGNAL_INL
