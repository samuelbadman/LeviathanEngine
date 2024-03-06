#pragma once

#include "MathTypes.h"
#include "MathLibrary.h"

namespace LeviathanRenderer
{
	enum class ProjectionMode : unsigned char
	{
		Orthographic,
		Perspective
	};

	class Camera
	{
	private:
		LeviathanCore::MathTypes::Vector3 Position = {};
		LeviathanCore::MathTypes::Euler Orientation = {};
		LeviathanCore::MathTypes::Matrix4x4 ViewMatrix = {};
		LeviathanCore::MathTypes::Matrix4x4 ProjectionMatrix = {};
		LeviathanCore::MathTypes::Matrix4x4 ViewProjectionMatrix = {};

		ProjectionMode Projection = ProjectionMode::Perspective;
		float NearZ = 0.1f;
		float FarZ = 1000.0f;

		float FovYRadians = LeviathanCore::MathLibrary::DegreesToRadians(90.0f);

		float OrthoWidth = 5e-3f;

	public:
		inline void SetPosition(const LeviathanCore::MathTypes::Vector3& position) { Position = position; }
		inline void SetOrientation(const LeviathanCore::MathTypes::Euler& orientation) { Orientation = orientation; }
		inline void SetProjectionMode(const ProjectionMode mode) { Projection = mode; }
		inline void SetNearZ(float nearZ) { NearZ = nearZ; }
		inline void SetFarZ(float farZ) { FarZ = farZ; }
		inline void SetFovY(float fovYDegrees) { FovYRadians = LeviathanCore::MathLibrary::DegreesToRadians(fovYDegrees); }
		inline void SetOrthoWidth(float orthoWidth) { OrthoWidth = orthoWidth; }

		void UpdateViewMatrix();
		void UpdateProjectionMatrix(int renderAreaWidth, int renderAreaHeight);
		void UpdateViewProjectionMatrix();

		inline const LeviathanCore::MathTypes::Matrix4x4& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }
	};
}