//
// Created by arthur on 14/03/2023.
//

#ifndef CONCERTO_CORE_MATH_EULERANGLES_HPP
#define CONCERTO_CORE_MATH_EULERANGLES_HPP

#include "Concerto/Core/Math/Vector/Vector.hpp"
#include "Concerto/Core/Stream/Stream.hpp"

namespace cct
{
	template<typename T>
	class Quaternion;

	/**
	 * @brief A class representing Euler angles
	 * @tparam T The type of the angles
	 */
	template<typename T>
	class EulerAngles
	{
	 public:
		EulerAngles() = default;
		/**
		 * @brief Construct a new EulerAngles object
		 * @param pitch The X axis
		 * @param yaw The Y axis
		 * @param roll The Z axis
		 */
		constexpr EulerAngles(T pitch, T yaw, T roll);

		/**
		 * @brief Get the pitch
		 * @return T A reference to pitch
		 */
		[[nodiscard]] constexpr T& Pitch();
		/**
		 * @brief Get the yaw
		 * @return T A reference to yaw
		 */
		[[nodiscard]] constexpr T& Yaw();

		/**
		 * @brief Get the roll
		 * @return T A reference to roll
		 */
		[[nodiscard]] constexpr T& Roll();

		/**
		 * @brief Get the pitch
		 * @return T The pitch
		 */
		[[nodiscard]] constexpr T Pitch() const;
		/**
		 * @brief Get the yaw
		 * @return T The yaw
		 */
		[[nodiscard]] constexpr T Yaw() const;

		/**
		 * @brief Get the roll
		 * @return T The roll
		 */
		[[nodiscard]] constexpr T Roll() const;

		/**
		 * @brief Convert the Euler angles to a quaternion
		 * @return Quaternion<T> The quaternion
		 */
		[[nodiscard]] constexpr Quaternion<T> ToQuaternion() const;

		constexpr EulerAngles<T> operator+(const EulerAngles<T>& other) const;
		constexpr EulerAngles<T> operator-(const EulerAngles<T>& other) const;
		constexpr EulerAngles<T> operator*(const EulerAngles<T>& other) const;
		constexpr EulerAngles<T> operator/(const EulerAngles<T>& other) const;
		constexpr EulerAngles<T>& operator+=(const EulerAngles<T>& other);
		constexpr EulerAngles<T>& operator-=(const EulerAngles<T>& other);
		constexpr EulerAngles<T>& operator*=(const EulerAngles<T>& other);
		constexpr EulerAngles<T>& operator/=(const EulerAngles<T>& other);

		constexpr EulerAngles<T> operator+(const Vector<T, 3>& other) const;
		constexpr EulerAngles<T> operator-(const Vector<T, 3>& other) const;
		constexpr EulerAngles<T> operator*(const Vector<T, 3>& other) const;
		constexpr EulerAngles<T> operator/(const Vector<T, 3>& other) const;
		constexpr EulerAngles<T>& operator+=(const Vector<T, 3>& other);
		constexpr EulerAngles<T>& operator-=(const Vector<T, 3>& other);
		constexpr EulerAngles<T>& operator*=(const Vector<T, 3>& other);
		constexpr EulerAngles<T>& operator/=(const Vector<T, 3>& other);

		/**
		 * @brief Get the zero Euler angles
		 * @return EulerAngles<T> The zero Euler angles
		 */
		[[nodiscard]] constexpr static EulerAngles<T> Zero();

		void Serialize(Stream& stream) const;
		void Deserialize(Stream& stream);

	 private:
		T _pitch;
		T _yaw;
		T _roll;
	};

	//Alias
	using EulerAnglesf = EulerAngles<float>;
	using EulerAnglesd = EulerAngles<double>;
}
#include "Concerto/Core/Math/EulerAngles/EulerAngles.inl"

#endif //CONCERTO_CORE_MATH_EULERANGLES_HPP
