#include "Camera.h"

namespace LeviathanRenderer
{
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