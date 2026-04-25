//
// Created by arthur on 27/02/2026.
//

#ifndef CONCERTO_CORE_CONNECTION_INL
#define CONCERTO_CORE_CONNECTION_INL

namespace cct
{
	inline void Connection::Disconnect()
	{
		if (m_disconnectFn)
		{
			m_disconnectFn();
			m_disconnectFn = nullptr;
		}
	}

	inline bool Connection::IsConnected() const
	{
		return m_disconnectFn && !m_weakSignal.expired();
	}

	inline void ScopedConnection::Disconnect()
	{
		m_connection.Disconnect();
	}

	inline bool ScopedConnection::IsConnected() const
	{
		return m_connection.IsConnected();
	}

	inline Connection ScopedConnection::Release()
	{
		Connection c = std::move(m_connection);
		m_connection = Connection{};
		return c;
	}
} // namespace cct

#endif // CONCERTO_CORE_CONNECTION_INL
