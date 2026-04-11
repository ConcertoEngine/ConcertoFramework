//
// Created by arthur on 22/08/2022.
//

#ifndef CONCERTO_CORE_MATH_MATRIX_HPP
#define CONCERTO_CORE_MATH_MATRIX_HPP

#include <array>
#include <concepts>

#include "Concerto/Core/Types/Types.hpp"

namespace cct
{
	/**
	 * @brief A template class representing a mathematical matrix.
	 * @attention The elements of the matrix are stored in column-major order.
	 * @tparam T The type of elements in the matrix.
	 * @tparam Rows The number of rows in the matrix.
	 * @tparam Columns The number of columns in the matrix.
	 */
	template<typename T, std::size_t Rows, std::size_t Columns>
	class Matrix
	{
	public:
		using Type = T;

		template<typename... Args>
		constexpr explicit Matrix(Args&&... args);

		constexpr Matrix();

		constexpr Matrix(const Matrix&) = default;

		constexpr Matrix(Matrix&&) noexcept = default;

		constexpr Matrix& operator=(const Matrix&) = default;

		constexpr Matrix& operator=(Matrix&&) noexcept = default;

		~Matrix() = default;

		[[nodiscard]] static constexpr std::size_t GetWidth() noexcept;

		[[nodiscard]] static constexpr std::size_t GetHeight() noexcept;

		[[nodiscard]] static constexpr std::size_t GetSize() noexcept;

		[[nodiscard]] constexpr T* Data() noexcept;

		[[nodiscard]] constexpr const T* Data() const noexcept;

		/**
		 * @brief Get the value of the matrix at the given position.
		 *
		 * @param row The row of the matrix.
		 * @param column The column of the matrix.
		 * @return Reference to the value of the matrix at the given position.
		 */
		constexpr T& GetElement(std::size_t row, std::size_t column) noexcept;

		/**
		 * @brief Get the value of the matrix at the given position.
		 *
		 * @param row The row of the matrix.
		 * @param column The column of the matrix.
		 * @return Constant reference to the value of the matrix at the given position.
		 */
		[[nodiscard]] constexpr const T& GetElement(std::size_t row, std::size_t column) const noexcept;

		/**
		 * @brief Creates an identity matrix.
		 * @attention The matrix must be square.
		 * @return Identity matrix.
		 */
		static constexpr Matrix Identity() noexcept;

		/**
		 * @brief Inverse the matrix.
		 * @return A new matrix that is the inverse of the matrix.
		 */
		[[nodiscard]] constexpr Matrix Inverse() const noexcept;

		/**
		 * @brief Add the given matrix to the current matrix.
		 * @param other The matrix to add.
		 * @return A new matrix that is the sum of the two matrices.
		 */
		constexpr Matrix operator+(const Matrix& other) const noexcept;

		/**
		 * @brief Subtract the given matrix to the current matrix.
		 * @param other The matrix to subtract.
		 * @return A new matrix that is the subtraction of the two matrices.
		 */
		constexpr Matrix operator-(const Matrix& other) const noexcept;

		/**
		 * @brief Multiply the given matrix to the current matrix.
		 * @param other The matrix to multiply.
		 * @return A new matrix that is the multiplication of the two matrices.
		 */
		constexpr Matrix operator*(const Matrix& other) const noexcept;

		/**
		 * @brief Divide the given matrix to the current matrix.
		 * @param other The matrix to divide.
		 * @return A new matrix that is the division of the two matrices.
		 */
		constexpr Matrix operator/(const Matrix& other) const noexcept;

		/**
		 * @brief Add the given value to the current matrix.
		 * @param value The value to add.
		 * @return A new matrix that is the sum of the matrix and the value.
		 */
		constexpr Matrix operator+(T value) const noexcept;

		/**
		 * @brief Subtract the given value to the current matrix.
		 * @param value The value to subtract.
		 * @return A new matrix that is the subtraction of the matrix and the value.
		 */
		constexpr Matrix operator-(T value) const noexcept;

		/**
		 * @brief Multiply the given value to the current matrix.
		 * @param value The value to multiply.
		 * @return A new matrix that is the multiplication of the matrix and the value.
		 */
		constexpr Matrix operator*(T value) const noexcept;

		/**
		 * @brief Divide the given value to the current matrix.
		 * @param value The value to divide.
		 * @return A new matrix that is the division of the matrix and the value.
		 */
		constexpr Matrix operator/(T value) const noexcept;

		/**
		 * @brief Add the given matrix to the current matrix.
		 * @param other The matrix to add.
		 * @return A reference to the current matrix.
		 */
		constexpr Matrix<T, Rows, Columns>& operator+=(const Matrix& other) noexcept;

		/**
		 * @brief Subtract the given matrix to the current matrix.
		 * @param other The matrix to subtract.
		 * @return A reference to the current matrix.
		 */
		constexpr Matrix& operator-=(const Matrix& other) noexcept;

		/**
		 * @brief Multiply the given matrix to the current matrix.
		 * @param other The matrix to multiply.
		 * @return A reference to the current matrix.
		 */
		constexpr Matrix& operator*=(const Matrix& other) noexcept;

		/**
		 * @brief Divide the given matrix to the current matrix.
		 * @param other The matrix to divide.
		 * @return A reference to the current matrix.
		 */
		constexpr Matrix& operator/=(const Matrix& other) noexcept;

		/**
		 * @brief Add the given value to the current matrix.
		 * @param value The value to add.
		 * @return A reference to the current matrix.
		 */
		constexpr Matrix& operator+=(T value) noexcept;

		/**
		 * @brief Subtract the given value to the current matrix.
		 * @param value The value to subtract.
		 * @return A reference to the current matrix.
		 */
		constexpr Matrix& operator-=(T value) noexcept;

		/**
		 * @brief Multiply the given value to the current matrix.
		 * @param value The value to multiply.
		 * @return A reference to the current matrix.
		 */
		constexpr Matrix& operator*=(T value) noexcept;

		/**
		 * @brief Divide the given value to the current matrix.
		 * @param value The value to divide.
		 * @return A reference to the current matrix.
		 */
		constexpr Matrix& operator/=(T value) noexcept;

		/**
		 * @brief Compare the current matrix with the given matrix.
		 * @param other The matrix to compare.
		 * @return True if the current matrix is equal to the given matrix, false otherwise.
		 */
		constexpr bool operator==(const Matrix& other) const;

		/**
		 * @brief Compare the current matrix with the given matrix.
		 * @param other The matrix to compare.
		 * @return False if the current matrix is equal to the given matrix, true otherwise.
		 */
		constexpr bool operator!=(const Matrix& other) const;

		/**
		 * @brief Get the value of the matrix at the given position.
		 *
		 * @param row The row of the matrix.
		 * @param column The column of the matrix.
		 * @return Reference to the value of the matrix at the given position.
		 */
		constexpr T& operator()(std::size_t row, std::size_t column) noexcept;

		/**
		 * @brief Get the value of the matrix at the given position.
		 *
		 * @param row The row of the matrix.
		 * @param column The column of the matrix.
		 * @return Constant reference to the value of the matrix at the given position.
		 */
		constexpr const T& operator()(std::size_t row, std::size_t column) const noexcept;

	private:
		std::array<T, Rows * Columns> _data;
	};

	// Aliases
	template<typename T>
	using Matrix4 = Matrix<T, 4, 4>;

	template<typename T>
	using Matrix3 = Matrix<T, 3, 3>;

	template<typename T>
	using Matrix2 = Matrix<T, 2, 2>;

	using Matrix4f = Matrix<float, 4, 4>;
	using Matrix3f = Matrix<float, 3, 3>;
	using Matrix2f = Matrix<float, 2, 2>;

	using Matrix4d = Matrix<double, 4, 4>;
	using Matrix3d = Matrix<double, 3, 3>;
	using Matrix2d = Matrix<double, 2, 2>;

	using Matrix4i = Matrix<Int32, 4, 4>;
	using Matrix3i = Matrix<Int32, 3, 3>;
	using Matrix2i = Matrix<Int32, 2, 2>;

	//Concepts
	template<typename M>
	concept Matrix3x3 = std::same_as<Matrix<typename M::Type, 3, 3>, M>;

	template<typename M>
	concept Matrix4x4 = std::same_as<Matrix<typename M::Type, 4, 4>, M>;

	template<typename M>
	concept Matrix3x4 = std::same_as<Matrix<typename M::Type, 3, 4>, M>;
}// namespace cct

#include "Concerto/Core/Math/Matrix/Matrix.inl"

#endif//CONCERTO_CORE_MATH_MATRIX_HPP
