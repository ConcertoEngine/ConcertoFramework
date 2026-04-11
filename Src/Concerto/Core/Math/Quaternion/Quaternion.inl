//
// Created by arthur on 10/02/2024.
//

#ifndef CONCERTO_CORE_MATH_QUATERNION_INL
#define CONCERTO_CORE_MATH_QUATERNION_INL

#include <cmath>

#include "Concerto/Core/Math/Algorithm.hpp"
#include "Concerto/Core/Math/Quaternion/Quaternion.hpp"

namespace cct
{
	template<typename T>
	constexpr Quaternion<T>::Quaternion(T x, T y, T z, T w) noexcept
		: _w(w), _x(x), _y(y), _z(z)
	{
	}

	template<typename T>
	constexpr Quaternion<T>::Quaternion(const EulerAngles<T>& eulerAngles) noexcept
		: _w(0), _x(0), _y(0), _z(0)
	{
		Set(eulerAngles.Pitch(), eulerAngles.Yaw(), eulerAngles.Roll());
	}

	template<typename T>
	constexpr T Quaternion<T>::X() const noexcept
	{
		return _x;
	}

	template<typename T>
	constexpr T Quaternion<T>::Y() const noexcept
	{
		return _y;
	}

	template<typename T>
	constexpr T Quaternion<T>::Z() const noexcept
	{
		return _z;
	}

	template<typename T>
	constexpr T Quaternion<T>::W() const noexcept
	{
		return _w;
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::Normalize()
	{
		auto res = *this;
		T length = Length();
		res._w /= length;
		res._x /= length;
		res._y /= length;
		res._z /= length;
		return res;
	}

	template<typename T>
	constexpr T Quaternion<T>::Magnitude() const
	{
		return _w * _w + _x * _x + _y * _y
			+ _z * _z;
	}

	template<typename T>
	constexpr T Quaternion<T>::Length() const
	{
		return std::sqrt(Magnitude());
	}

	template<typename T>
	constexpr EulerAngles<T> Quaternion<T>::ToEulerAngles() const
	{
		//According to http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
		T test = _x * _y + _z * _w;
		if (test > T(0.499))
			return EulerAngles<T>(Pi<T> / T(2), T(2) * std::atan2(_x, _w), T(0));
		if (test < T(-0.499))
			return EulerAngles<T>(-Pi<T>, T(-2) * std::atan2(_x, _w), T(0));
		T sqx = _x * _x;
		T sqy = _y * _y;
		T sqz = _z * _z;
		return EulerAngles<T>(
			ToDegrees(std::atan2(T(2.0) * _x * _w - T(2.0) * _y * _z, T(1.0) - T(2.0) * sqx - T(2.0) * sqz)),
			ToDegrees(std::atan2(T(2.0) * _y * _w - T(2.0) * _x * _z, T(1.0) - T(2.0) * sqy - T(2.0) * sqz)),
			ToDegrees(std::asin(T(2) * test)));
	}

	template<typename T>
	constexpr Quaternion<T>& Quaternion<T>::operator=(const Vector3<T>& vector) noexcept
	{
		Set(vector.X(), vector.Y(), vector.Z());
		return *this;
	}

	template<typename T>
	constexpr void Quaternion<T>::Set(T pitch, T yaw, T roll) noexcept
	{
		T cosRoll = std::cos(ToRadians(roll / T(2.0)));
		T cosPitch = std::cos(ToRadians(pitch / T(2.0)));
		T cosYaw = std::cos(ToRadians(yaw / T(2.0)));

		T sinRoll = std::sin(ToRadians(roll / T(2.0)));
		T sinPitch = std::sin(ToRadians(pitch / T(2.0)));
		T sinYaw = std::sin(ToRadians(yaw / T(2.0)));

		_w = cosYaw * cosRoll * cosPitch - sinYaw * sinRoll * sinPitch;
		_x = sinYaw * sinRoll * cosPitch + cosYaw * cosRoll * sinPitch;
		_y = sinYaw * cosRoll * cosPitch + cosYaw * sinRoll * sinPitch;
		_z = cosYaw * sinRoll * cosPitch - sinYaw * cosRoll * sinPitch;
	}

	template<typename T>
	constexpr T& Quaternion<T>::W() noexcept
	{
		return _w;
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::FromAxisAngle(const Vector3<T>& axis, const T angle)
	{
		const T halfAngle = ToRadians(angle) * T(0.5);
		const auto normalizedAxis = axis.Normalize();
		const T sinHalfAngle = std::sin(halfAngle);

		return Quaternion<T>(
			axis.X() * sinHalfAngle,
			axis.Y() * sinHalfAngle,
			axis.Z() * sinHalfAngle,
			std::cos(halfAngle)
			);
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::Inverse() const noexcept
	{
		return Conjugate() / Length();
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::Conjugate() const noexcept
	{
		Quaternion<T> q = *this;
		q._x = -q.X();
		q._y = -q.Y();
		q._z = -q.Z();
		return q;
	}

	template<typename T>
	constexpr Quaternion<T>& Quaternion<T>::Conjugate() noexcept
	{
		_x = -_x;
		_y = -_y;
		_z = -_z;
		return *this;
	}

	template<typename T>
	template<typename M>
		requires Matrix3x3<M> || Matrix3x4<M> || Matrix4x4<M>
	constexpr M Quaternion<T>::ToRotationMatrix() const
	{
		//According to http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/
		M matrix = M::Identity();
		matrix(0, 0) = T(1.0) - T(2.0) * (_y * _y) - T(2.0) * _z * _z;
		matrix(0, 1) = T(2.0) * _x * _y - T(2.0) * _z * _w;
		matrix(0, 2) = T(2.0) * _x * _z + T(2.0) * _y * _w;

		matrix(1, 0) = T(2.0) * _x * _y + T(2.0) * _z * _w;
		matrix(1, 1) = T(1.0) - T(2.0) * _x * _x - T(2.0) *_z * _z;
		matrix(1, 2) = T(2.0) * _y * _z - T(2.0) *_x * _w;
		matrix(1, 3) = T(0.0);

		matrix(2, 0) = T(2.0) * _x * _z  - T(2.0) * _y * _w;
		matrix(2, 1) = T(2.0) * _y * _z + T(2.0) * _x * _w;
		matrix(2, 2) = T(1.0) - T(2.0) * _x * _x - T(2.0) * _y * _y;
		matrix(2, 3) = T(0.0);

		return matrix;
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::operator+(const Quaternion<T>& other) const noexcept
	{
		return Quaternion<T>(_x + other._x, _y + other._y, _z + other._z,
		                     _w + other._w);
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::operator-(const Quaternion<T>& other) const noexcept
	{
		return Quaternion<T>(_x - other._x, _y - other._y, _z - other._z,
		                     _w - other._w);
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::operator*(const Quaternion<T>& other) const noexcept
	{
		Quaternion<T> result;
		result._x =
			_w * other._x + other._w * _x + _y * other._z -
			_z * other._y;
		result._y =
			_w * other._y + other._w * _y + _z * other._x -
			_x * other._z;
		result._z =
			_w * other._z + other._w * _z + _x * other._y -
			_y * other._x;
		result._w =
			_w * other._w - _x * other._x - _y * other._y -
			_z * other._z;
		return result;
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::operator/(const Quaternion<T>& other) const noexcept
	{
		return *this * other.Conjugate();
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::operator+(T value) const noexcept
	{
		return Quaternion<T>(_x + value, _y + value, _z + value,
		                     _w + value);
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::operator-(T value) const noexcept
	{
		return Quaternion<T>(_x - value, _y - value, _z - value,
		                     _w - value);
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::operator*(T value) const noexcept
	{
		return Quaternion<T>(_x * value, _y * value, _z * value,
		                     _w * value);
	}

	template<typename T>
	constexpr Quaternion<T> Quaternion<T>::operator/(T value) const noexcept
	{
		return Quaternion<T>(_x / value, _y / value, _z / value,
		                     _w / value);
	}

	template<typename T>
	constexpr Quaternion<T>& Quaternion<T>::operator+=(const Quaternion<T>& other) noexcept
	{
		_x += other._x;
		_y += other._y;
		_z += other._z;
		_w += other._w;
		return *this;
	}

	template<typename T>
	constexpr Quaternion<T>& Quaternion<T>::operator-=(const Quaternion<T>& other) noexcept
	{
		_x -= other._x;
		_y -= other._y;
		_z -= other._z;
		_w -= other._w;
		return *this;
	}

	template<typename T>
	constexpr Quaternion<T>& Quaternion<T>::operator*=(const Quaternion<T>& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	template<typename T>
	constexpr Quaternion<T>& Quaternion<T>::operator/=(const Quaternion<T>& other) noexcept
	{
		*this = *this / other;
		return *this;
	}

	template<typename T>
	constexpr Vector3<T> Quaternion<T>::operator*(const Vector3<T>& vector) const noexcept
	{
		Vector3<T> q(_x, _y, _z);
		Vector3<T> uv = q.Cross(vector);
		Vector3<T> uuv = q.Cross(uv);
		uv *= T(2.0) * _w;
		uuv *= T(2.0);
		return vector + uv + uuv;
	}

	template<typename T>
	constexpr bool Quaternion<T>::operator==(const Quaternion<T>& other) const noexcept
	{
		return _x == other._x && _y == other._y && _z == other._z &&
			_w == other._w;
	}

	template<typename T>
	constexpr bool Quaternion<T>::operator!=(const Quaternion<T>& other) const noexcept
	{
		return !(*this == other);
	}

		template<typename T>
	inline void Quaternion<T>::Serialize(Stream& stream) const
	{
		cct::Serialize(stream, _w);
		cct::Serialize(stream, _x);
		cct::Serialize(stream, _y);
		cct::Serialize(stream, _z);
	}

	template<typename T>
	inline void Quaternion<T>::Deserialize(Stream& stream)
	{
		cct::Deserialize(stream, _w);
		cct::Deserialize(stream, _x);
		cct::Deserialize(stream, _y);
		cct::Deserialize(stream, _z);
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const Quaternion<T>& quaternion) noexcept
	{
		stream << "Quaternion(X: " << quaternion.X() << ", Y: " << quaternion.Y() << ", Z: " << quaternion.Z()
			<< ", W: " << quaternion.W() << ")";
		return stream;
	}
}// namespace cct

#endif//CONCERTO_CORE_MATH_QUATERNION_INL
