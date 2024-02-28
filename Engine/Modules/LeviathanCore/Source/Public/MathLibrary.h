#pragma once

namespace LeviathanCore
{
	namespace MathLibrary
	{
		static constexpr float Pi = 3.14159265359f;

		// Converts a degree value to radians.
		constexpr float DegreesToRadians(const float degrees)
		{
			return degrees * LeviathanCore::MathLibrary::Pi / 180.0f;
		}
		// Converts a radians value to degrees.
		constexpr float RadiansToDegrees(const float radians)
		{
			return radians * 180.0f / LeviathanCore::MathLibrary::Pi;
		}
	}
}