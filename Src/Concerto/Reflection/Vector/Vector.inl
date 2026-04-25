//
// Created by arthur on 27/02/2026.
//

#ifndef CONCERTO_REFLECTION_VECTOR_INL
#define CONCERTO_REFLECTION_VECTOR_INL

#include <Concerto/Core/Assert.hpp>

namespace cct::refl
{
	template<typename T>
	T* Vector::Get(std::size_t index)
	{
		return dynamic_cast<T*>(Get(index));
	}

	template<typename T>
	const T* Vector::Get(std::size_t index) const
	{
		return dynamic_cast<const T*>(Get(index));
	}

	inline auto Vector::begin() noexcept
	{
		return m_elements.begin();
	}

	inline auto Vector::end() noexcept
	{
		return m_elements.end();
	}

	inline auto Vector::begin() const noexcept
	{
		return m_elements.begin();
	}

	inline auto Vector::end() const noexcept
	{
		return m_elements.end();
	}
} // namespace cct::refl

#endif // CONCERTO_REFLECTION_VECTOR_INL
