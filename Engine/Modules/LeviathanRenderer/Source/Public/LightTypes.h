#pragma once
#include "MathTypes.h"
#include "MathLibrary.h"

namespace LeviathanRenderer
{
	namespace LightTypes
	{
		struct DirectionalLight
		{
			LeviathanCore::MathTypes::Vector3 Color{ 1.0f, 1.0f, 1.0f };
			float Brightness = 1.0f;
			// Direction in world space.
			LeviathanCore::MathTypes::Vector3 Direction{ -0.577350259f, -0.577350259f, 0.577350259f };
		};

		struct PointLight
		{
			LeviathanCore::MathTypes::Vector3 Color{ 1.0f, 1.0f, 1.0f };
			float Brightness = 1.0f;
			// Position in world space.
			LeviathanCore::MathTypes::Vector3 Position{ 0.0f, 0.0f, 0.0f };
		};

		struct SpotLight
		{
			LeviathanCore::MathTypes::Vector3 Color{ 1.0f, 1.0f, 1.0f };
			float Brightness = 1.0f;
			// Position in world space.
			LeviathanCore::MathTypes::Vector3 Position{ 0.0f, 0.0f, 0.0f };
			// Direction in world space.
			LeviathanCore::MathTypes::Vector3 Direction{ 0.0f, -1.0f, 0.0f };
			float InnerConeAngleRadians = LeviathanCore::MathLibrary::DegreesToRadians(0.0f);
			float OuterConeAngleRadians = LeviathanCore::MathLibrary::DegreesToRadians(17.5f);
		};
	}
}