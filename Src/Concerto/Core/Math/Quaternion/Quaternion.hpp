//
// Created by arthur on 03/09/2022.
//

#ifndef CONCERTO_CORE_MATH_QUATERNION_HPP
#define CONCERTO_CORE_MATH_QUATERNION_HPP

#include "Concerto/Core/Math/Vector/Vector.hpp"
#include "Concerto/Core/Math/EulerAngles/EulerAngles.hpp"
#include "Concerto/Core/Math/Matrix/Matrix.hpp"

namespace cct
{
	template<typename T>
	class Quaternion
	{
	public:
		Quaternion() = default;

		/**
		 * @brief Construct a new Quaternion object
		 *
		 * @param x The x
		 * @param y The y
		 * @param z The z
		 * @param w The w scalar
		 */
		constexpr Quaternion(T x, T y, T z, T w) noexcept;

		/**
		 * @brief Construct a new Quaternion object from euler angles
		 * @param eulerAngles The euler angles
		 */
		explicit constexpr Quaternion(const EulerAngles<T>& eulerAngles) noexcept;

		/**
		 * @brief Normalize the quaternion
		 */
		constexpr Quaternion<T> Normalize();

		/**
		 * @brief Calculates the Magnitude of the quaternion
		 * @return T The Magnitude
		 */
		[[nodiscard]] constexpr T Magnitude() const;

		/**
		 * @brief Calculates the length of the quaternion
		 * @return T The length
		 */
		[[nodiscard]] constexpr T Length() const;

		/**
		 * @brief Convert a quaternion to euler angles
		 * @attention The quaternion must be normalized
		 * @return Vector3<T> The euler angles
		 */
		[[nodiscard]] constexpr EulerAngles<T> ToEulerAngles() const;

		/**
		 * @brief Assign a new value to this quaternion
		 * @param vector The vector
		 * @return Quaternion& A reference to this quaternion
		 */
		constexpr Quaternion<T>& operator=(const Vector3<T>& vector) noexcept;

		/**
		 * @brief Assign a new value to this quaternion
		 */
		constexpr void Set(T pitch, T yaw, T roll) noexcept;

		/**
		 * @brief Get the w scalar
		 * @return The reference to the w scalar
		 */
		constexpr T& W() noexcept;

		/**
		 * @brief Get the x axis
		 * @return The x axis value
		 */
		[[nodiscard]] constexpr T X() const noexcept;

		/**
		 * @brief Get the y axis
		 * @return The y axis value
		 */
		[[nodiscard]] constexpr T Y() const noexcept;

		/**
		 * @brief Get the z axis
		 * @return The z axis value
		 */
		[[nodiscard]] constexpr T Z() const noexcept;

		/**
		 * @brief Get the w scalar
		 * @return The w scalar value
		 */
		[[nodiscard]] constexpr T W() const noexcept;

		/**
		 * @brief Construct a new Quaternion object from axis-angle representation
		 * @param axis The rotation axis
		 * @param angle The rotation angle, in degrees
		 * @return Quaternion<T> The new quaternion
		 */
		constexpr static Quaternion<T> FromAxisAngle(const Vector3<T>& axis, const T angle);

		/**
		 * @brief Get the inverse of this quaternion
		 * @return Quaternion The inverse of this quaternion
		 */
		[[nodiscard]] constexpr Quaternion<T> Inverse() const noexcept;

		/**
		 * @brief Conjugate quaternion
		 * @return A new quaternion that is the conjugate of the current one
		 */
		constexpr Quaternion<T> Conjugate() const noexcept;

		/**
		 * @brief Conjugate quaternion
		 * @return The quaternion's conjugate
		 */
		constexpr Quaternion<T>& Conjugate() noexcept;

		/**
		 * @brief Convert the quaternion to the rotation matrix
		 * @tparam M The matrix type, only Matrix3x3, Matrix3x4, Matrix4x4 are allowed
		 * @return M The rotation matrix
		 */
		template<typename M>
			requires Matrix3x3<M> || Matrix3x4<M> || Matrix4x4<M>
		[[nodiscard]] constexpr M ToRotationMatrix() const;

		/**
		 * @brief Add the given quaternion to the current one
		 * @param other The quaternion to add
		 * @return A new quaternion that is the sum of the two
		 */
		constexpr Quaternion<T> operator+(const Quaternion<T>& other) const noexcept;

		/**
		 * @brief Subtract the given quaternion to the current one
		 * @param other The quaternion to subtract
		 * @return A new quaternion that is the subtraction of the two
		 */
		constexpr Quaternion<T> operator-(const Quaternion<T>& other) const noexcept;

		/**
		 * @brief Multiply the given quaternion to the current one
		 * @param other The quaternion to multiply
		 * @attention Quaternion multiplication is not commutative
		 * @return A new quaternion that is the multiplication of the two
		 */
		constexpr Quaternion<T> operator*(const Quaternion<T>& other) const noexcept;

		/**
		 * @brief Divide the given quaternion to the current one
		 * @param other The quaternion to divide
		 * @return A new quaternion that is the division of the two
		 */
		constexpr Quaternion<T> operator/(const Quaternion<T>& other) const noexcept;

		/**
		 * @brief Add the given value to the current quaternion
		 * @param value The value to add
		 * @return A new quaternion that is the sum of the quaternion and the value
		 */
		constexpr Quaternion<T> operator+(T value) const noexcept;

		/**
		 * @brief Subtract the given value to the current quaternion
		 * @param value The value to subtract
		 * @return A new quaternion that is the subtraction of the quaternion and the value
		 */

		constexpr Quaternion<T> operator-(T value) const noexcept;

		/**
		 * @brief Multiply the given value to the current quaternion
		 * @param value The value to multiply
		 * @return A new quaternion that is the multiplication of the quaternion and the value
		 */

		constexpr Quaternion<T> operator*(T value) const noexcept;

		/**
		 * @brief Divide the given value to the current quaternion
		 * @param value The value to divide
		 * @return A new quaternion that is the division of the quaternion and the value
		 */

		constexpr Quaternion<T> operator/(T value) const noexcept;

		/**
		 * @brief Add the given quaternion to the current one
		 * @param other The quaternion to add
		 * @return A reference to the current quaternion
		 */
		constexpr Quaternion<T>& operator+=(const Quaternion<T>& other) noexcept;

		/**
		 * @brief Subtract the given quaternion to the current one
		 * @param other The quaternion to subtract
		 * @return A reference to the current quaternion
		 */
		constexpr Quaternion<T>& operator-=(const Quaternion<T>& other) noexcept;

		/**
		 * @brief Multiply the given quaternion to the current one
		 * @param other The quaternion to multiply
		 * @return A reference to the current quaternion
		 */
		constexpr Quaternion<T>& operator*=(const Quaternion<T>& other) noexcept;

		/**
		 * @brief Divide the given quaternion to the current one
		 * @param other The quaternion to divide
		 * @return A reference to the current quaternion
		 */
		constexpr Quaternion<T>& operator/=(const Quaternion<T>& other) noexcept;

		/**
		 * @brief Apply the quaternion to the given vector3D
		 * @param vector The vector to apply the quaternion to
		 * @return A new vector that is the result of the quaternion application
		 */
		constexpr Vector3<T> operator*(const Vector3<T>& vector) const noexcept;

		/**
		 * @brief Check if the quaternion is equal to the given one
		 * @param other The quaternion to compare to
		 * @return True if the quaternion is equal to the given one, false otherwise
		 */
		constexpr bool operator==(const Quaternion<T>& other) const noexcept;

		/**
		 * @brief Check if the quaternion is not equal to the given one
		 * @param other The quaternion to compare to
		 * @return True if the quaternion is not equal to the given one, false otherwise
		 */
		constexpr bool operator!=(const Quaternion<T>& other) const noexcept;

		void Serialize(Stream& stream) const;
		void Deserialize(Stream& stream);

	private:
		T _w;
		T _x;
		T _y;
		T _z;
	};

	// Aliases
	using Quaternionf = Quaternion<float>;
	using Quaterniond = Quaternion<double>;

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const Quaternion<T>& quaternion) noexcept;

}// namespace cct

#include "Concerto/Core/Math/Quaternion/Quaternion.inl"

#endif //CONCERTO_CORE_MATH_QUATERNION_HPP
