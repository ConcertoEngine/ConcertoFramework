//
// Created by arthur on 21/03/2023.
//

#ifndef CONCERTO_GRAPHICS_CAMERA_HPP
#define CONCERTO_GRAPHICS_CAMERA_HPP

#include <Concerto/Core/Math/Quaternion/Quaternion.hpp>
#include <Concerto/Core/Math/Vector/Vector.hpp>

#include "Concerto/Graphics/Core/Defines.hpp"
#include "Concerto/Graphics/Core/Camera/GPUData.hpp"

namespace cct::gfx
{
	class CONCERTO_GRAPHICS_CORE_API Camera : public GPUCamera
	{
	 public:
		enum class CameraMovement
		{
			Forward,
			Backward,
			Left,
			Right
		};

		Camera(float fov, float near, float far, float aspectRatio);
		Camera(const Camera&) = default;
		Camera(Camera&&) = default;
		Camera& operator=(const Camera&) = default;
		Camera& operator=(Camera&&) = default;

		[[nodiscard]] const EulerAnglesf& GetRotation() const;
		[[nodiscard]] const Vector4f& GetClearColor() const;
		[[nodiscard]] Vector3f GetPosition() const;
		[[nodiscard]] float GetFov() const;
		[[nodiscard]] float GetNear() const;
		[[nodiscard]] float GetFar() const;
		[[nodiscard]] float GetAspectRatio() const;

		void SetClearColor(const Vector4f& clearColor);
		void SetPosition(const Vector3f& position);
		void SetFov(float fov);
		void SetNear(float near);
		void SetFar(float far);
		void SetAspectRatio(float aspectRatio);

		void UpdateViewProjectionMatrix();
		void UpdateProjectionMatrix();

		void Rotate(double deltaX, double deltaY);
		void Move(CameraMovement direction, float x);

	private:
		EulerAnglesf m_eulerAngles;
		Vector3f m_velocity;
		Vector3f m_position;
		float m_movementSpeed;
		float m_mouseSensitivity;
		float m_fov;
		float m_near;
		float m_far;
		float m_aspectRatio;
		Vector4f m_clearColor;
	};
}

#endif //CONCERTO_GRAPHICS_CAMERA_HPP
