//
// Created by arthur on 22/08/2022.
//

#ifndef CONCERTO_CORE_MATH_MATRIX_INL
#define CONCERTO_CORE_MATH_MATRIX_INL

#include "Concerto/Core/Assert.hpp"
#include "Concerto/Core/Math/Matrix/Matrix.hpp"

namespace cct
{
	template<typename T, std::size_t Rows, std::size_t Columns>
	template<typename... Args>
	constexpr Matrix<T, Rows, Columns>::Matrix(Args&&... args)
	{
		static_assert(sizeof...(args) == Rows * Columns,
					  "Error : The number of arguments doesn't match the matrix size");
		T data[Rows][Columns] = {std::forward<Args>(args)...};
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				_data[j * Rows + i] = data[i][j];
			}
		}
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns>::Matrix()
		: _data({0})
	{
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr std::size_t Matrix<T, Rows, Columns>::GetWidth() noexcept
	{
		return Columns;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr std::size_t Matrix<T, Rows, Columns>::GetHeight() noexcept
	{
		return Rows;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr std::size_t Matrix<T, Rows, Columns>::GetSize() noexcept
	{
		return Columns * Rows;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr T* Matrix<T, Rows, Columns>::Data() noexcept
	{
		return _data.data();
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr const T* Matrix<T, Rows, Columns>::Data() const noexcept
	{
		return _data.data();
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr T& Matrix<T, Rows, Columns>::GetElement(std::size_t row, std::size_t column) noexcept
	{
		CCT_ASSERT(row < Rows, "Invalid size");
		CCT_ASSERT(column < Columns, "Invalid size");
		return _data[column * Rows + row];
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr const T& Matrix<T, Rows, Columns>::GetElement(std::size_t row, std::size_t column) const noexcept
	{
		CCT_ASSERT(row < Rows, "Invalid size");
		CCT_ASSERT(column < Columns, "Invalid size");
		return _data[column * Rows + row];
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::Identity() noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				result.GetElement(i, j) = i == j ? T(1) : T(0);
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::Inverse() const noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				result.GetElement(i, j) = GetElement(j, i);
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::operator+(const Matrix<T, Rows, Columns>& other) const noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				result(i, j) = (*this)(i, j) + other(i, j);
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::operator-(const Matrix<T, Rows, Columns>& other) const noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				result.GetElement(i, j) = GetElement(i, j) - other.GetElement(i, j);
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::operator*(const Matrix<T, Rows, Columns>& other) const noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				for (std::size_t k = 0; k < Columns; ++k)
				{
					result.GetElement(i, j) += GetElement(i, k) * other.GetElement(k, j);
				}
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::operator/(const Matrix& other) const noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				result.GetElement(i, j) = GetElement(i, j) / other.GetElement(i, j);
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::operator+(T value) const noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				result.GetElement(i, j) = GetElement(i, j) + value;
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::operator-(T value) const noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				result.GetElement(i, j) = GetElement(i, j) - value;
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::operator*(T value) const noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				result.GetElement(i, j) = GetElement(i, j) * value;
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns> Matrix<T, Rows, Columns>::operator/(T value) const noexcept
	{
		Matrix result;
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				result.GetElement(i, j) = GetElement(i, j) / value;
			}
		}
		return result;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns>& Matrix<T, Rows, Columns>::operator+=(const Matrix<T, Rows, Columns>& other) noexcept
	{
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				GetElement(i, j) += other.GetElement(i, j);
			}
		}
		return *this;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns>& Matrix<T, Rows, Columns>::operator-=(const Matrix<T, Rows, Columns>& other) noexcept
	{
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				GetElement(i, j) -= other.GetElement(i, j);
			}
		}
		return *this;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns>& Matrix<T, Rows, Columns>::operator*=(const Matrix<T, Rows, Columns>& other) noexcept
	{
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				GetElement(i, j) *= other.GetElement(i, j);
			}
		}
		return *this;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns>& Matrix<T, Rows, Columns>::operator/=(const Matrix<T, Rows, Columns>& other) noexcept
	{
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				GetElement(i, j) /= other.GetElement(i, j);
			}
		}
		return *this;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns>& Matrix<T, Rows, Columns>::operator+=(T value) noexcept
	{
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				GetElement(i, j) += value;
			}
		}
		return *this;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns>& Matrix<T, Rows, Columns>::operator-=(T value) noexcept
	{
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				GetElement(i, j) -= value;
			}
		}
		return *this;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns>& Matrix<T, Rows, Columns>::operator*=(T value) noexcept
	{
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				GetElement(i, j) *= value;
			}
		}
		return *this;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr Matrix<T, Rows, Columns>& Matrix<T, Rows, Columns>::operator/=(T value) noexcept
	{
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				GetElement(i, j) /= value;
			}
		}
		return *this;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr bool Matrix<T, Rows, Columns>::operator==(const Matrix<T, Rows, Columns>& other) const
	{
		for (std::size_t i = 0; i < Rows; ++i)
		{
			for (std::size_t j = 0; j < Columns; ++j)
			{
				if (GetElement(i, j) != other.GetElement(i, j))
				{
					return false;
				}
			}
		}
		return true;
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr bool Matrix<T, Rows, Columns>::operator!=(const Matrix<T, Rows, Columns>& other) const
	{
		return !(*this == other);
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr T& Matrix<T, Rows, Columns>::operator()(std::size_t row, std::size_t column) noexcept
	{
		return GetElement(row, column);
	}

	template<typename T, std::size_t Rows, std::size_t Columns>
	constexpr const T& Matrix<T, Rows, Columns>::operator()(std::size_t row, std::size_t column) const noexcept
	{
		return GetElement(row, column);
	}
}// namespace cct
#endif//CONCERTO_CORE_MATH_MATRIX_INL
