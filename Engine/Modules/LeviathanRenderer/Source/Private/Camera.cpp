#include "Camera.h"

#include "Logging.h"

namespace LeviathanRenderer
{
    void Camera::AddYawRotation(const float yawDeltaRadians)
    {
        Orientation.SetYawRadians(Orientation.YawRadians() + yawDeltaRadians);
    }

    void Camera::AddPitchRotation(const float pitchDeltaRadians)
    {
        float newPitchRadians = Orientation.PitchRadians() + pitchDeltaRadians;

        if (newPitchRadians < MinPitchRadians)
        {
            newPitchRadians = MinPitchRadians;
        }
        else if (newPitchRadians > MaxPitchRadians)
        {
            newPitchRadians = MaxPitchRadians;
        }

        Orientation.SetPitchRadians(newPitchRadians);
    }

    void Camera::UpdateViewMatrix()
    {
        ViewMatrix = LeviathanCore::MathTypes::Matrix4x4::View(Position, Orientation);
    }

    void Camera::UpdateProjectionMatrix(int renderAreaWidth, int renderAreaHeight)
    {
        switch (Projection)
        {
        case ProjectionMode::Orthographic:
            ProjectionMatrix = LeviathanCore::MathTypes::Matrix4x4::OrthographicProjection(static_cast<float>(renderAreaWidth) * OrthoWidth,
                static_cast<float>(renderAreaHeight) * OrthoWidth, NearZ, FarZ);
            break;

        case ProjectionMode::Perspective:
            ProjectionMatrix = LeviathanCore::MathTypes::Matrix4x4::PerspectiveProjection(FovYRadians, static_cast<float>(renderAreaWidth) / static_cast<float>(renderAreaHeight), 
                NearZ, FarZ);
            break;
        }
    }

    void Camera::UpdateViewProjectionMatrix()
    {
        ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
    }

    LeviathanCore::MathTypes::Vector3 Camera::GetForwardVector(const LeviathanCore::MathTypes::Vector3& baseForward)
    {
        return LeviathanCore::MathTypes::Quaternion(Orientation) * baseForward;
    }

    LeviathanCore::MathTypes::Vector3 Camera::GetRightVector(const LeviathanCore::MathTypes::Vector3& baseRight)
    {
        return LeviathanCore::MathTypes::Quaternion(Orientation) * baseRight;
    }

    LeviathanCore::MathTypes::Vector3 Camera::GetUpVector(const LeviathanCore::MathTypes::Vector3& baseUp)
    {
        return LeviathanCore::MathTypes::Quaternion(Orientation) * baseUp;
    }
}