//
// Created by arthur on 22/05/22.
//

#include "Concerto/Core/Math/Transform/Transform.hpp"

namespace cct
{

	Transform::Transform(const Vector3f& location, const Quaternionf& rotation, const Vector3f& scale) :
		_location(location), _rotation(rotation), _scale(scale)
	{
	}

	void Transform::Translate(float x, float y, float z)
	{
		_location.X() += x;
		_location.Y() += y;
		_location.Z() += z;
	}

	void Transform::Translate(const Vector3f& vec)
	{
		Translate(vec.X(), vec.Y(), vec.Z());
	}

	void Transform::Rotate(const Quaternionf& rotation)
	{
		_rotation *= rotation;
	}

	void Transform::Scale(const Vector3f& scale)
	{
		_scale *= scale;
	}

	const Vector3f& Transform::GetLocation() const
	{
		return _location;
	}

	const Quaternionf& Transform::GetRotation() const
	{
		return _rotation;
	}

	const Vector3f& Transform::GetScale() const
	{
		return _scale;
	}

	bool Transform::operator==(const Transform& other) const
	{
		return _location == other._location && _rotation == other._rotation && _scale == other._scale;
	}

	bool Transform::operator!=(const Transform& other) const
	{
		return (*this == other);
	}

	void Transform::Serialize(Stream& stream) const
	{
		_location.Serialize(stream);
		_rotation.Serialize(stream);
		_scale.Serialize(stream);
	}

	void Transform::Deserialize(Stream& stream)
	{
		_location.Deserialize(stream);
		_rotation.Deserialize(stream);
		_scale.Deserialize(stream);
	}
}