//
// Created by arthur on 04/09/2022.
//

#ifndef CONCERTO_CORE_MATH_ALGORITHM_HPP
#define CONCERTO_CORE_MATH_ALGORITHM_HPP

namespace cct
{
	/**
	 * @brief The Pi constant
	 * @tparam T The type of the constant

	 */
	template<typename T>
	constexpr T Pi = T(3.1415926535897932384626433832795);

	/**
	 * @brief Convert degrees to radians
	 * @param degrees The degrees
	 * @return T The radians
	 */
	template<typename T>
	constexpr T ToRadians(T degrees) noexcept
	{
		return degrees * Pi<T> / T(180);
	}

	/**
	 * @brief Convert radians to degrees
	 * @param radians The radians
	 * @return T The degrees
	 */
	template<typename T>
	constexpr T ToDegrees(T radians) noexcept
	{
		return radians * T(180) / Pi<T>;
	}
}
#endif //CONCERTO_CORE_MATH_ALGORITHM_HPP
