#include "Camera.h"

#include "Logging.h"

namespace LeviathanRenderer
{
    void Camera::AddYawRotation(const float yawDeltaRadians)
    {
        Orientation.SetYawRadians(Orientation.GetYawRadians() + yawDeltaRadians);
    }

    void Camera::AddPitchRotation(const float pitchDeltaRadians)
    {
        float newPitchRadians = Orientation.GetPitchRadians() + pitchDeltaRadians;

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
        ViewMatrix.TransposeInPlace();
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

        ProjectionMatrix.TransposeInPlace();
    }

    void Camera::UpdateViewProjectionMatrix()
    {
        ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
    }
}