//
// Created by arthur on 14/03/2023.
//

#ifndef CONCERTO_CORE_MATH_EULERANGLES_INL
#define CONCERTO_CORE_MATH_EULERANGLES_INL

#include "Concerto/Core/Math/EulerAngles/EulerAngles.hpp"
#include "Concerto/Core/Math/Quaternion/Quaternion.hpp"
#include "Concerto/Core/Serializer/Serializer.hpp"

namespace cct
{
	template<typename T>
	constexpr EulerAngles<T>::EulerAngles(T pitch, T yaw, T roll)
		: _pitch(pitch), _yaw(yaw), _roll(roll)
	{

	}

	template<typename T>
	constexpr T& EulerAngles<T>::Pitch()
	{
		return _pitch;
	}

	template<typename T>
	constexpr T& EulerAngles<T>::Yaw()
	{
		return _yaw;
	}

	template<typename T>
	constexpr T& EulerAngles<T>::Roll()
	{
		return _roll;
	}

	template<typename T>
	constexpr T EulerAngles<T>::Pitch() const
	{
		return _pitch;
	}

	template<typename T>
	constexpr T EulerAngles<T>::Yaw() const
	{
		return _yaw;
	}

	template<typename T>
	constexpr T EulerAngles<T>::Roll() const
	{
		return _roll;
	}

	template<typename T>
	constexpr Quaternion<T> EulerAngles<T>::ToQuaternion() const
	{
		return Quaternion<T>(*this);
	}

	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator+(const EulerAngles<T>& other) const
	{
		return EulerAngles<T>(
			_pitch + other._pitch,
			_yaw + other._yaw,
			_roll + other._roll
		);
	}

	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator-(const EulerAngles<T>& other) const
	{
		return EulerAngles<T>(
			_pitch - other._pitch,
			_yaw - other._yaw,
			_roll - other._roll
		);
	}

	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator*(const EulerAngles<T>& other) const
	{
		return EulerAngles<T>(
			_pitch * other._pitch,
			_yaw * other._yaw,
			_roll * other._roll
		);
	}

	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator/(const EulerAngles<T>& other) const
	{
		return EulerAngles<T>(
			_pitch / other._pitch,
			_yaw / other._yaw,
			_roll / other._roll
		);
	}

	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator+=(const EulerAngles<T>& other)
	{
		_pitch += other._pitch;
		_yaw += other._yaw;
		_roll += other._roll;
		return *this;
	}

	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator-=(const EulerAngles<T>& other)
	{
		_pitch -= other._pitch;
		_yaw -= other._yaw;
		_roll -= other._roll;
		return *this;
	}

	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator*=(const EulerAngles<T>& other)
	{
		_pitch *= other._pitch;
		_yaw *= other._yaw;
		_roll *= other._roll;
		return *this;
	}

	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator/=(const EulerAngles<T>& other)
	{
		_pitch /= other._pitch;
		_yaw /= other._yaw;
		_roll /= other._roll;
		return *this;
	}

	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator+(const Vector<T, 3>& other) const
	{
		EulerAngles<T> result(*this);
		result += other;
		return result;
	}

	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator-(const Vector<T, 3>& other) const
	{
		EulerAngles<T> result(*this);
		result -= other;
		return result;
	}

	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator*(const Vector<T, 3>& other) const
	{
		EulerAngles<T> result(*this);
		result *= other;
		return result;
	}

	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator/(const Vector<T, 3>& other) const
	{
		EulerAngles<T> result(*this);
		result /= other;
		return result;
	}

	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator+=(const Vector<T, 3>& other)
	{
		_pitch += other.X();
		_yaw += other.Y();
		_roll += other.Z();
		return *this;
	}

	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator-=(const Vector<T, 3>& other)
	{
		_pitch -= other.X();
		_yaw -= other.Y();
		_roll -= other.Z();
		return *this;
	}

	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator*=(const Vector<T, 3>& other)
	{
		_pitch *= other.X();
		_yaw *= other.Y();
		_roll *= other.Z();
		return *this;
	}

	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator/=(const Vector<T, 3>& other)
	{
		_pitch /= other.X();
		_yaw /= other.Y();
		_roll /= other.Z();
		return *this;
	}

	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::Zero()
	{
		return EulerAngles<T>(0, 0, 0);
	}

	template<typename T>
	void EulerAngles<T>::Serialize(Stream& stream) const
	{
		cct::Serialize(stream, _pitch);
		cct::Serialize(stream, _yaw);
		cct::Serialize(stream, _roll);
	}

	template<typename T>
	void EulerAngles<T>::Deserialize(Stream& stream)
	{
		cct::Deserialize(stream, _pitch);
		cct::Deserialize(stream, _yaw);
		cct::Deserialize(stream, _roll);
	}
}

#endif//CONCERTO_CORE_MATH_EULERANGLES_INL