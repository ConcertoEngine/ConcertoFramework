//
// Created by arthur on 27/02/2026.
//

#ifndef CONCERTO_CORE_CONNECTION_HPP
#define CONCERTO_CORE_CONNECTION_HPP

#include <functional>
#include <memory>

namespace cct
{
	/// Non-owning handle to a signal connection.
	/// Call Disconnect() to manually remove the slot from the signal.
	/// Safe to use after the Signal is destroyed (becomes a no-op).
	class Connection
	{
	public:
		Connection() = default;
		Connection(const Connection&) = default;
		Connection(Connection&&) = default;
		Connection& operator=(const Connection&) = default;
		Connection& operator=(Connection&&) = default;
		~Connection() = default;

		inline void Disconnect();
		[[nodiscard]] inline bool IsConnected() const;

	private:
		explicit Connection(std::function<void()> disconnectFn, std::weak_ptr<void> weakSignal) :
			m_disconnectFn(std::move(disconnectFn)),
			m_weakSignal(std::move(weakSignal))
		{
		}

		template<typename... Args>
		friend class Signal;

		std::function<void()> m_disconnectFn;
		std::weak_ptr<void> m_weakSignal;
	};

	/// RAII wrapper around Connection. Automatically disconnects when destroyed.
	/// Movable, non-copyable.
	class ScopedConnection
	{
	public:
		ScopedConnection() = default;
		explicit ScopedConnection(Connection connection) :
			m_connection(std::move(connection))
		{
		}
		~ScopedConnection() { m_connection.Disconnect(); }

		ScopedConnection(const ScopedConnection&) = delete;
		ScopedConnection& operator=(const ScopedConnection&) = delete;

		ScopedConnection(ScopedConnection&&) = default;
		ScopedConnection& operator=(ScopedConnection&& other) noexcept
		{
			if (this != &other)
			{
				m_connection.Disconnect();
				m_connection = std::move(other.m_connection);
			}
			return *this;
		}

		inline void Disconnect();
		[[nodiscard]] inline bool IsConnected() const;
		[[nodiscard]] inline Connection Release();

	private:
		Connection m_connection;
	};
} // namespace cct

#include "Concerto/Core/Signal/Connection.inl"

#endif // CONCERTO_CORE_CONNECTION_HPP
