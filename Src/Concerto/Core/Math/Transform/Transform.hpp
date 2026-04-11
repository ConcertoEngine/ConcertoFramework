//
// Created by arthur on 22/05/22.
//

#ifndef CONCERTO_CORE__MATH_TRANSFORM_HPP
#define CONCERTO_CORE__MATH_TRANSFORM_HPP

#include "Concerto/Core/Types/Types.hpp"
#include "Concerto/Core/Math/Vector/Vector.hpp"
#include "Concerto/Core/Math/Quaternion/Quaternion.hpp"

namespace cct
{
	class CCT_CORE_PUBLIC_API Transform
	{
	public:
		Transform() = default;
		/**
		 * @brief Construct a new Transform object
		 * @param location The location
		 * @param rotation The rotation
		 * @param scale The scale
		 */
		Transform(const Vector3f& location, const Quaternionf& rotation, const Vector3f& scale);
		Transform(Transform&&) = default;
		Transform(const Transform&) = default;
		Transform& operator=(Transform&&) = default;
		Transform& operator=(const Transform&) = default;
		~Transform() = default;

		/**
		 * @brief Translate the transform by the given vector
		 * @param x The X axis translation
		 * @param y The Y axis translation
		 * @param z The Z axis translation
		 */
		void Translate(float x, float y, float z);

		/**
		 * @brief Translate the transform by the given vector
		 * @param vec The translation vector
		 */
		void Translate(const Vector3f& vec);

		/**
		 * @brief Rotate the transform by the given quaternion
		 * @param rotation The rotation quaternion
		 */
		void Rotate(const Quaternionf& rotation);

		/**
		 * @brief Scale the transform by the given vector
		 * @param scale The scale vector
		 */
		void Scale(const Vector3f& scale);

		/**
		 * @brief Get the location of the transform
		 * @return The location
		 */
		[[nodiscard]] const Vector3f& GetLocation() const;

		/**
		 * @brief Get the rotation of the transform
		 * @return The rotation
		 */
		[[nodiscard]] const Quaternionf& GetRotation() const;

		/**
		 * @brief Get the scale of the transform
		 * @return The scale
		 */
		[[nodiscard]] const Vector3f& GetScale() const;

		bool operator==(const Transform& other) const;
		bool operator!=(const Transform& other) const;

		void Serialize(Stream& stream) const;
		void Deserialize(Stream& stream);

	private:
		Vector3f _location;
		Quaternionf _rotation;
		Vector3f _scale;
	};
}// namespace cct
#endif//CONCERTO_CORE__MATH_TRANSFORM_HPP