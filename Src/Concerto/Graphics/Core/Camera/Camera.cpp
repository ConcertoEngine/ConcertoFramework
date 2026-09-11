//
// Created by arthur on 21/03/2023.
//

#include "Concerto/Graphics/Core/Camera/Camera.hpp"

namespace cct::gfx
{
	Camera::Camera(float fov, float near, float far, float aspectRatio) :
		m_eulerAngles(),
		m_velocity(),
		m_position(),
		m_fov(fov),
		m_near(near),
		m_far(far),
		m_aspectRatio(aspectRatio),
		m_clearColor(0.0f, 0.0f, 0.0f, 1.0f)
	{
		UpdateProjectionMatrix();
	}

	const EulerAnglesf& Camera::GetRotation() const
	{
		return m_eulerAngles;
	}

	const Vector4f& Camera::GetClearColor() const
	{
		return m_clearColor;
	}

	Vector3f Camera::GetPosition() const
	{
		return m_position;
	}

	float Camera::GetFov() const
	{
		return m_fov;
	}

	float Camera::GetNear() const
	{
		return m_near;
	}

	float Camera::GetFar() const
	{
		return m_far;
	}

	float Camera::GetAspectRatio() const
	{
		return m_aspectRatio;
	}

	const Matrix4f& Camera::GetViewMatrix() const
	{
		return m_viewMatrix;
	}

	const Matrix4f& Camera::GetProjectionMatrix() const
	{
		return m_projectionMatrix;
	}

	const Matrix4f& Camera::GetViewProjectionMatrix() const
	{
		return m_viewProjectionMatrix;
	}

	GPUCamera Camera::ToGPUCamera() const
	{
		return GPUCamera{m_viewMatrix, m_projectionMatrix, m_viewProjectionMatrix};
	}

	void Camera::SetClearColor(const Vector4f& clearColor)
	{
		m_clearColor = clearColor;
	}

	void Camera::SetPosition(const Vector3f& position)
	{
		m_position = Vector3f(position.X(), position.Y(), position.Z());
	}

	void Camera::SetFov(float fov)
	{
		m_fov = fov;
		UpdateProjectionMatrix();
	}

	void Camera::SetNear(float near)
	{
		m_near = near;
		UpdateProjectionMatrix();
	}

	void Camera::SetFar(float far)
	{
		m_far = far;
		UpdateProjectionMatrix();
	}

	void Camera::SetAspectRatio(float aspectRatio)
	{
		m_aspectRatio = aspectRatio;
		UpdateProjectionMatrix();
	}

	Vector<float, 3> operator*(const Matrix4f& mat, const Vector4f& vec)
	{
		Vector<float, 3> result;

		for (std::size_t widthIndex = 0; widthIndex < mat.GetWidth(); ++widthIndex)
		{
			const std::size_t finalWidthIndex = widthIndex * mat.GetHeight();

			for (std::size_t heightIndex = 0; heightIndex < mat.GetHeight(); ++heightIndex)
				result[heightIndex] += mat.Data()[finalWidthIndex + heightIndex] * vec[widthIndex];
		}

		return result;
	}

	void Camera::UpdateViewProjectionMatrix()
	{
		const Matrix4f translation = m_position.ToTranslationMatrix();
		const Matrix4f cameraRotation = m_eulerAngles.ToQuaternion().ToRotationMatrix<Matrix4f>();
		m_viewMatrix = (translation * cameraRotation).Inverse();
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void Camera::UpdateProjectionMatrix()
	{
		const float tanHalfFov = std::tan(m_fov / 2.0f);
		m_projectionMatrix = Matrix4f();
		m_projectionMatrix(0, 0) = 1.f / (m_aspectRatio * tanHalfFov);
		m_projectionMatrix(1, 1) = 1.f / tanHalfFov;
		m_projectionMatrix(2, 2) = m_far / (m_near - m_far);
		m_projectionMatrix(2, 3) = -(m_far * m_near) / (m_far - m_near);
		m_projectionMatrix(3, 2) = -1.f;
	}

	void Camera::Rotate(double deltaX, double deltaY)
	{
		m_eulerAngles.Yaw() -= deltaX;
		m_eulerAngles.Pitch() += deltaY;

		if (m_eulerAngles.Pitch() >= 89.0f)
			m_eulerAngles.Pitch() = 89.0f;
		if (m_eulerAngles.Pitch() <= -89.0f)
			m_eulerAngles.Pitch() = -89.0f;
	}

	void Camera::Move(CameraMovement direction, float x)
	{
		switch (direction)
		{
			case CameraMovement::Forward:
				m_position += m_eulerAngles.ToQuaternion() * Vector3f::Forward() * x;
				break;
			case CameraMovement::Backward:
				m_position += m_eulerAngles.ToQuaternion() * Vector3f::Backward() * x;
				break;
			case CameraMovement::Left:
				m_position += m_eulerAngles.ToQuaternion() * Vector3f::Left() * x;
				break;
			case CameraMovement::Right:
				m_position += m_eulerAngles.ToQuaternion() * Vector3f::Right() * x;
				break;
		}
	}
} // namespace cct::gfx
