//
// Created by arthur on 22/05/22.
//

#ifndef CONCERTO_CORE_MATH_VECTOR_HPP
#define CONCERTO_CORE_MATH_VECTOR_HPP


#include <array>
#include <iostream>
#include <type_traits>
#include <cmath>

#include "Concerto/Core/Types/Types.hpp"
#include "Concerto/Core/Stream/Stream.hpp"
#include "Concerto/Core/Math/Matrix/Matrix.hpp"

namespace cct
{
	/**
	 * @brief Vector class that represents a mathematical vector.
	 *
	 * @tparam T The type of the vector's data.
	 * @tparam Size The size of the vector
	 */
	template<typename T, std::size_t Size>
	class Vector
	{
	public:
		Vector() = default;

		template<typename... Args,
			typename = std::enable_if_t<sizeof...(Args) == Size>>
			explicit Vector(Args&& ... args) : _data{ static_cast<T>(args)... }
		{
		}

		Vector(Vector&&) noexcept = default;

		Vector(const Vector&) = default;

		Vector& operator=(Vector&&) noexcept = default;

		Vector& operator=(const Vector&) = default;

		~Vector() = default;

		/**
		 * @brief Get the vector's data
		 * @return The vector's data
		 */
		[[nodiscard]] const std::array<T, Size>& GetData() const;

		/**
		 * @brief Get the size of the vector
		 * @return The size of the vector
		 */
		[[nodiscard]] static constexpr std::size_t GetSize()
		{
			return Size;
		}

		/**
		 * @brief Get the X axis of the vector (GetData()[0])
		 * @return A copy of the X axis
		 */
		[[nodiscard]] constexpr T& X() const noexcept
		{
			static_assert(Size >= 1, "Error : Getting the X value require the vector size 1 or more");
			return const_cast<T&>(_data[0]);
		}

		/**
		 * @brief Get the Y axis of the vector (GetData()[1])
		 * @return A copy of the Y axis
		 */
		[[nodiscard]] constexpr T Y() const noexcept
		{
			static_assert(Size >= 2, "Error : Getting the Y value require the vector size 2 or more");
			return _data[1];
		}

		/**
		 * @brief Get the Z axis of the vector (GetData()[2])
		 * @return A copy of the Z axis
		 */
		[[nodiscard]] constexpr T Z() const noexcept
		{
			static_assert(Size >= 3, "Error : Getting the Z value require the vector size 3 or more");
			return _data[2];
		}

		/**
		 * @brief Get the X axis of the vector (GetData()[0])
		 * @return A reference to the X axis
		 */
		[[nodiscard]] T& X() noexcept
		{
			static_assert(Size >= 1, "Error : Getting the X value require the vector size 1 or more");
			return _data[0];
		}

		/**
		 * @brief Get the Y axis of the vector (GetData()[1])
		 * @return A reference to the Y axis
		 */
		[[nodiscard]] T& Y() noexcept
		{
			static_assert(Size >= 2, "Error : Getting the Y value require the vector size 2 or more");
			return _data[1];
		}

		/**
		 * @brief Get the Z axis of the vector (GetData()[2])
		 * @return A reference to the Z axis
		 */
		[[nodiscard]] T& Z() noexcept
		{
			static_assert(Size >= 3, "Error : Getting the Z value require the vector size 3 or more");
			return _data[2];
		}

		/**
		 * @brief Create a translation matrix from the vector
		 * @return A translation matrix
		 */
		Matrix4<T> ToTranslationMatrix() const noexcept;

		/**
		 * @brief Create a scaling matrix from the vector
		 * @return A scaling matrix
		 */
		Matrix4<T> ToScalingMatrix() const noexcept requires(Size == 3 || Size == 4);

		/**
		 * @brief Computes the dot product of two vectors
		 * @param other The other vector
		 * @return The dot product
		 */
		[[nodiscard]] constexpr T Dot(const Vector& other) const noexcept
		{
			T result = 0;
			for (std::size_t i = 0; i < Size; ++i)
			{
				result += _data[i] * other._data[i];
			}
			return result;
		}

		/**
		 * @brief Computes the cross product of two vectors
		 * @attention This function is only available for 3D vectors
		 * @param other The other vector
		 * @return A new vector that is the cross product of the two vectors
		 */
		[[nodiscard]] constexpr Vector<T, Size> Cross(const Vector& other) const noexcept
		{
			static_assert(Size == 3, "Error : Cross product require a vector of size 3");
			Vector<T, Size> result;
			result.X() = Y() * other.Z() - Z() * other.Y();
			result.Y() = Z() * other.X() - X() * other.Z();
			result.Z() = X() * other.Y() - Y() * other.X();
			return result;
		}

		/**
		 * @brief Normalize the vector
		 * @return A new vector that is the normalized version of the vector
		 */
		[[nodiscard]] constexpr Vector<T, Size> Normalize() const noexcept
		{
			return *this / Length();
		}

		/**
		 * @brief Get the length of the vector
		 * @return The length of the vector
		 */
		[[nodiscard]] constexpr T Length() const noexcept
		{
			return std::sqrt(Dot(*this));
		}

		[[nodiscard]] constexpr Vector<T, Size> Reflect(const Vector& normal) const noexcept
		{
			return *this - normal * 2 * Dot(normal);
		}

		constexpr Vector operator+(const Vector& vector) const noexcept;

		constexpr Vector operator+(T val) const noexcept;

		constexpr Vector operator-(const Vector& vector) const noexcept;

		constexpr Vector operator-(T val) const noexcept;

		constexpr Vector operator*(const Vector& vector) const noexcept;

		constexpr Vector operator*(T val) const noexcept;

		constexpr Vector operator/(const Vector& vector) const noexcept;

		constexpr Vector operator/(T val) const noexcept;

		constexpr Vector& operator+=(const Vector& vector) noexcept;

		constexpr Vector& operator+=(T val) noexcept;

		constexpr Vector& operator-=(const Vector& vector) noexcept;

		constexpr Vector& operator-=(T val) noexcept;

		constexpr Vector& operator*=(const Vector& vector) noexcept;

		constexpr Vector& operator*=(T val) noexcept;

		constexpr Vector& operator/=(const Vector& vector) noexcept;

		constexpr Vector& operator/=(T val) noexcept;

		constexpr const T& operator[](std::size_t index) const
		{
			return _data[index];
		}

		constexpr T& operator[](std::size_t index)
		{
			return _data[index];
		}

		constexpr bool operator==(const Vector& other) const noexcept;

		constexpr bool operator!=(const Vector& other) const noexcept
		{
			return *this != other;
		}

		/**
		 * @brief Shorthand for writing Vector<T,Size>(0,0,...)
		 * @return A new vector with all axis set to 0
		 */
		static Vector<T, Size> Zero() noexcept
		{
			Vector<T, Size> result;
			for (std::size_t i = 0; i < Size; ++i)
			{
				result[i] = 0;
			}
			return result;
		}

		/**
		 * @brief Shorthand for writing Vector<T,Size>(1,1,...)
		 * @return A new vector with all axis set to 1
		 */
		static Vector<T, Size> One() noexcept
		{
			Vector<T, Size> result;
			for (std::size_t i = 0; i < Size; ++i)
			{
				result[i] = 1;
			}
			return result;
		}

		/**
		 * @brief Shorthand for writing Vector<T,Size>(1,0,...)
		 * @return A new vector with first axis set to 1 and the others to 0
		 */
		static Vector<T, Size> Right() noexcept
		{
			Vector<T, Size> result;
			for (std::size_t i = 0; i < Size; ++i)
			{
				result[i] = 0;
			}
			result[0] = 1;
			return result;
		}

		/**
		 * @brief Shorthand for writing Vector<T,Size>(-1,0,...)
		 * @return A new vector with first axis set to -1 and the others to 0
		 */
		static Vector<T, Size> Left() noexcept
		{
			Vector<T, Size> result;
			for (std::size_t i = 0; i < Size; ++i)
			{
				result[i] = 0;
			}
			result[0] = -1;
			return result;
		}

		/**
		 * @brief Shorthand for writing Vector<T,Size>(0,1,...)
		 * @return A new vector with second axis set to 1 and the others to 0
		 */
		static Vector<T, Size> Up() noexcept
		{
			Vector<T, Size> result;
			for (std::size_t i = 0; i < Size; ++i)
			{
				result[i] = 0;
			}
			result[1] = 1;
			return result;
		}

		/**
		 * @brief Shorthand for writing Vector<T,Size>(0,-1,...)
		 * @return A new vector with second axis set to -1 and the others to 0
		 */

		static Vector<T, Size> Down() noexcept
		{
			Vector<T, Size> result;
			for (std::size_t i = 0; i < Size; ++i)
			{
				result[i] = 0;
			}
			result[1] = -1;
			return result;
		}

		/**
		 * @brief Shorthand for writing Vector<T,Size>(0,0,1)
		 * @return A new vector with last axis set to 1 and the others to 0
		 */
		static Vector<T, Size> Forward() noexcept
		{
			Vector<T, Size> result;
			for (std::size_t i = 0; i < Size; ++i)
			{
				result[i] = 0;
			}
			result[Size - 1] = 1;
			return result;
		}

		/**
		 * @brief Shorthand for writing Vector<T,Size>(...,1)
		 * @return A new vector with last axis set to -1 and the others to 0
		 */
		static Vector<T, Size> Backward() noexcept
		{
			Vector<T, Size> result;
			for (std::size_t i = 0; i < Size; ++i)
			{
				result[i] = 0;
			}
			result[Size - 1] = -1;
			return result;
		}

		void Serialize(Stream& stream) const;
		void Deserialize(Stream& stream);
	private:
		std::array<T, Size> _data;
	};

	template<typename T, std::size_t Size>
	constexpr std::ostream& operator<<(std::ostream& os, const Vector<T, Size>& vector)
	{
		os << "Vector" << Size << '(';
		for (std::size_t i = 0; i < Size; ++i)
		{
			os << vector[i];
			if (i != Size - 1)
			{
				os << ", ";
			}
		}
		os << ")";
		return os;
	}

	using Vector2i = Vector<Int32, 2>;
	using Vector2d = Vector<double, 2>;
	using Vector2f = Vector<float, 2>;
	using Vector2u = Vector<UInt32, 2>;
	template<typename T>
	using Vector2 = Vector<T, 2>;

	using Vector3i = Vector<Int32, 3>;
	using Vector3d = Vector<double, 3>;
	using Vector3f = Vector<float, 3>;
	using Vector3u = Vector<UInt32, 3>;
	template<typename T>
	using Vector3 = Vector<T, 3>;

	using Vector4i = Vector<Int32, 4>;
	using Vector4d = Vector<double, 4>;
	using Vector4f = Vector<float, 4>;
	using Vector4u = Vector<UInt32, 4>;
	template<typename T>
	using Vector4 = Vector<T, 4>;

	template<typename T> constexpr Vector3<T> operator*(T scale, const Vector3<T>& vec);
	template<typename T> constexpr Vector3<T> operator/(T scale, const Vector3<T>& vec);
}

#include "Concerto/Core/Math/Vector/Vector.inl"

#endif //CONCERTO_CORE_MATH_VECTOR_HPP