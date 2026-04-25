//
// Created by arthur on 27/02/2026.
//

#ifndef CONCERTO_CORE_SIGNAL_HPP
#define CONCERTO_CORE_SIGNAL_HPP

#include <functional>
#include <memory>
#include <vector>

#include "Concerto/Core/Signal/Connection.hpp"

namespace cct
{
	/// Type-safe signal that can be connected to zero or more slots (callbacks).
	///
	/// - Lazy allocation: no heap allocation until the first Connect() call.
	/// - Move-safe: connections remain valid after a Signal is moved.
	/// - Copy-safe: copying a Signal produces a Signal with NO connections (fresh).
	/// - Reentrant-safe: disconnecting inside Emit() is deferred until after emission.
	/// - Lifetime-safe: Connection::Disconnect() is a no-op if the Signal was destroyed.
	///
	/// Usage:
	///   Signal<int> onChanged;
	///   auto conn = onChanged.Connect([](int v) { ... });
	///   auto conn2 = onChanged.Connect(myObj, &MyClass::OnChanged);
	///   onChanged.Emit(42);
	///   conn.Disconnect();
	template<typename... Args>
	class Signal
	{
	public:
		Signal() = default;

		Signal(const Signal&) :
			m_state(nullptr)
		{
		}
		Signal& operator=(const Signal&)
		{
			return *this; // intentionally do not copy connections
		}

		Signal(Signal&&) = default;
		Signal& operator=(Signal&&) = default;

		~Signal() = default;

		[[nodiscard]] Connection Connect(std::function<void(Args...)> slot);
		template<typename T>
		[[nodiscard]] Connection Connect(T* obj, void (T::*method)(Args...));
		template<typename T>
		[[nodiscard]] Connection Connect(const T* obj, void (T::*method)(Args...) const);

		void Disconnect(Connection& connection);
		void DisconnectAll();

		void Emit(Args... args) const;
		void operator()(Args... args) const;

		[[nodiscard]] std::size_t GetConnectionCount() const;
		[[nodiscard]] bool HasConnections() const;

	private:
		struct Slot
		{
			std::size_t id;
			std::function<void(Args...)> fn;
			bool active = true;
		};

		struct SlotList
		{
			std::vector<Slot> slots;
			std::size_t nextId = 0;
			int emitDepth = 0;
		};

		void EnsureState() const;
		void DisconnectById(std::size_t id) const;

		mutable std::shared_ptr<SlotList> m_state; // null until first Connect()
	};
} // namespace cct

#include "Concerto/Core/Signal/Signal.inl"

#endif // CONCERTO_CORE_SIGNAL_HPP
