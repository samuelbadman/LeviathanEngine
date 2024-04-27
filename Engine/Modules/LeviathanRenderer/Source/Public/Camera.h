#pragma once

#include "MathTypes.h"
#include "MathLibrary.h"

namespace LeviathanRenderer
{
	class Camera
	{
	public:
		enum class ProjectionMode : unsigned char
		{
			Orthographic,
			Perspective
		};

	private:
		LeviathanCore::MathTypes::Vector3 Position = {};
		LeviathanCore::MathTypes::Euler Orientation = {};
		LeviathanCore::MathTypes::Matrix4x4 ViewMatrix = {};
		LeviathanCore::MathTypes::Matrix4x4 ProjectionMatrix = {};
		LeviathanCore::MathTypes::Matrix4x4 ViewProjectionMatrix = {};

		ProjectionMode Projection = ProjectionMode::Perspective;
		float NearZ = 0.1f;
		float FarZ = 1000.0f;
		float MinPitchRadians = -LeviathanCore::MathLibrary::HalfPi;
		float MaxPitchRadians = LeviathanCore::MathLibrary::HalfPi;

		float FovYRadians = LeviathanCore::MathLibrary::DegreesToRadians(45.0f);

		float OrthoWidth = 0.005f;

	public:
		inline void SetPosition(const LeviathanCore::MathTypes::Vector3& position) { Position = position; }
		inline void SetOrientation(const LeviathanCore::MathTypes::Euler& orientation) { Orientation = orientation; }
		inline void SetProjectionMode(const ProjectionMode mode) { Projection = mode; }
		inline void SetNearZ(float nearZ) { NearZ = nearZ; }
		inline void SetFarZ(float farZ) { FarZ = farZ; }
		inline void SetFovY(float fovYDegrees) { FovYRadians = LeviathanCore::MathLibrary::DegreesToRadians(fovYDegrees); }
		inline void SetOrthoWidth(float orthoWidth) { OrthoWidth = orthoWidth; }
		inline const LeviathanCore::MathTypes::Vector3& GetPosition() const { return Position; }
		inline const LeviathanCore::MathTypes::Euler& GetOrientation() const { return Orientation; }
		inline const LeviathanCore::MathTypes::Matrix4x4& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }

		void AddYawRotation(const float yawDeltaRadians);
		void AddPitchRotation(const float pitchDeltaRadians);

		void UpdateViewMatrix();
		void UpdateProjectionMatrix(int renderAreaWidth, int renderAreaHeight);
		void UpdateViewProjectionMatrix();

		LeviathanCore::MathTypes::Vector3 GetForwardVector(const LeviathanCore::MathTypes::Vector3& baseForward);
		LeviathanCore::MathTypes::Vector3 GetRightVector(const LeviathanCore::MathTypes::Vector3& baseRight);
		LeviathanCore::MathTypes::Vector3 GetUpVector(const LeviathanCore::MathTypes::Vector3& baseUp);
	};
}