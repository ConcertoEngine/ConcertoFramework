//
// Created by arthur on 13/09/2026.
//

#ifndef CONCERTO_CORE_TRACKABLE_HPP
#define CONCERTO_CORE_TRACKABLE_HPP

#include <memory>

namespace cct
{
	class Trackable
	{
	public:
		Trackable() = default;
		Trackable(const Trackable&)
		{
		}
		Trackable(Trackable&& other) noexcept :
			m_aliveToken(std::move(other.m_aliveToken))
		{
		}
		Trackable& operator=(const Trackable&) noexcept
		{
			return *this;
		}
		Trackable& operator=(Trackable&& other) noexcept
		{
			if (this != &other)
			{
				m_aliveToken = std::move(other.m_aliveToken);
			}
			return *this;
		}
		~Trackable() = default;

	private:
		template<typename...>
		friend class Signal;

		[[nodiscard]] std::weak_ptr<void> GetTrackToken() const
		{
			return m_aliveToken;
		}

		std::shared_ptr<void> m_aliveToken = std::make_shared<char>('\0');
	};
} // namespace cct

#endif // CONCERTO_CORE_TRACKABLE_HPP
