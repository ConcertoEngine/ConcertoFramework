//
// Created by arthur on 22/05/22.
//

#include <algorithm>

#include "Concerto/Core/Math/Vector/Vector.hpp"

namespace cct
{
	template<typename T, std::size_t Size>
	constexpr Vector<T, Size> Vector<T, Size>::operator+(const Vector& vector) const noexcept
	{
		auto res = *this;
		res += vector;
		return res;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size> Vector<T, Size>::operator+(T val) const noexcept
	{
		auto res = *this;
		res += val;
		return res;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size> Vector<T, Size>::operator-(const Vector& vector) const noexcept
	{
		auto res = *this;
		res -= vector;
		return res;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size> Vector<T, Size>::operator-(T val) const noexcept
	{
		auto res = *this;
		res -= val;
		return res;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size> Vector<T, Size>::operator*(const Vector& vector) const noexcept
	{
		auto res = *this;
		res *= vector;
		return res;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size> Vector<T, Size>::operator*(T val) const noexcept
	{
		auto res = *this;
		res *= val;
		return res;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size> Vector<T, Size>::operator/(const Vector& vector) const noexcept
	{
		auto res = *this;
		res /= vector;
		return res;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size> Vector<T, Size>::operator/(T val) const noexcept
	{
		auto res = *this;
		res /= val;
		return res;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size>& Vector<T, Size>::operator+=(const Vector& vector) noexcept
	{
		for (std::size_t i = 0; i < std::min(Size, vector.GetSize()); ++i)
			_data[i] += vector[i];
		return *this;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size>& Vector<T, Size>::operator+=(T val) noexcept
	{
		for (auto& x: _data)
			x += val;
		return *this;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size>& Vector<T, Size>::operator-=(const Vector& vector) noexcept
	{
		for (int i = 0; i < std::min(Size, vector.GetSize()); ++i)
			_data[i] -= vector[i];
		return *this;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size>& Vector<T, Size>::operator-=(T val) noexcept
	{
		for (auto& x: _data)
			x -= val;
		return *this;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size>& Vector<T, Size>::operator*=(const Vector& vector) noexcept
	{
		for (std::size_t i = 0; i < std::min(Size, vector.GetSize()); ++i)
			_data[i] *= vector[i];
		return *this;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size>& Vector<T, Size>::operator*=(T val) noexcept
	{
		for (auto& x: _data)
			x *= val;
		return *this;
	}

	template<typename T, std::size_t Size>
	constexpr Vector<T, Size>& Vector<T, Size>::operator/=(const Vector& vector) noexcept
	{
		for (int i = 0; i < std::min(Size, vector.GetSize()); ++i)
			_data[i] /= vector[i];
		return *this;
	}


	template<typename T, std::size_t Size>
	constexpr Vector<T, Size>& Vector<T, Size>::operator/=(T val) noexcept
	{
		for (auto& x: _data)
			x /= val;
		return *this;
	}

	template<typename T, std::size_t Size>
	constexpr bool Vector<T, Size>::operator==(const Vector& other) const noexcept
	{
		return _data == other.GetData();
	}

	template<typename T>
	constexpr Vector3<T> operator*(T scale, const Vector3<T>& vec)
	{
		return Vector3<T>(scale * vec.X(), scale * vec.Y(), scale * vec.Z());
	}

	template<typename T>
	constexpr Vector3<T> operator/(T scale, const Vector3<T>& vec)
	{
		return Vector3<T>(scale / vec.X(), scale / vec.Y(), scale / vec.Z());
	}

	template<typename T, std::size_t Size>
	inline void Vector<T, Size>::Serialize(Stream& stream) const
	{
		for (const T& x : _data)
			cct::Serialize(stream, x);
	}

	template<typename T, std::size_t Size>
	inline void Vector<T, Size>::Deserialize(Stream& stream)
	{
		for (T& x : _data)
			cct::Deserialize(stream, x);
	}

	template<typename T, std::size_t Size>
	const std::array<T, Size>& Vector<T, Size>::GetData() const
	{
		return _data;
	}

	template<typename T, std::size_t Size>
	Matrix4<T> Vector<T, Size>::ToTranslationMatrix() const noexcept
	{
		Matrix4<T> res = Matrix4<T>::Identity();
		res(3, 0) += X();
		res(3, 1) += Y();
		res(3, 2) += Z();
		return res;
	}

	template<typename T, std::size_t Size>
	Matrix4<T> Vector<T, Size>::ToScalingMatrix() const noexcept
		requires(Size == 3 || Size == 4)
	{
		Matrix4<T> res = Matrix4<T>::Identity();
		for (std::size_t i = 0; i < Size; ++i)
			res(i, i) = _data[i];
		return res;
	}
}
