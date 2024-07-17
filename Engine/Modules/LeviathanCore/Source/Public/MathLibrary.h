#pragma once

namespace LeviathanCore
{
	namespace MathLibrary
	{
		static constexpr float Pi = 3.14159265359f;
		static constexpr const char* PiString = "3.14159265359";
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
		float WrapAngleRadians(const float angleRadians);

		// Returns -1 when value is negative, 1 when value is greater than 0 and 0 when value is 0.
		template <typename T>
		constexpr char Sign(T value)
		{
			return (T(0) < value) - (value < T(0));
		}

		// Returns the value squared.
		template <typename T>
		constexpr T Square(T value)
		{
			return value * value;
		}

		template <typename T>
		constexpr T Cos(T radians)
		{
			return cos(radians);
		}

		template <typename T>
		constexpr T Sin(T radians)
		{
			return sin(radians);
		}

		template <typename T>
		constexpr T ATan2(T y, T x)
		{
			return atan2(y, x);
		}

		template <typename T>
		constexpr T Clamp(T x, T min, T max)
		{
			return std::max(min, std::min(x, max));
		}
	}
}