#pragma once

namespace LeviathanCore
{
	namespace MathLibrary
	{
		static constexpr float Pi = 3.14159265359f;
		static constexpr float HalfPi = 0.5f * Pi; // 90 degrees.
		static constexpr float TwoPi = 2.0f * Pi; // 180 degrees.

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

		// Wraps the angle in radians between 0 to 2Pi radians (0 to 360 degrees).
		float WrapAngle(const float angleRadians);
	}
}