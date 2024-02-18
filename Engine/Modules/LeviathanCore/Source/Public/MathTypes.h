#pragma once

namespace LeviathanCore
{
	namespace MathTypes
	{
		class Vector3
		{
		private:
			static constexpr size_t XComponent = 0;
			static constexpr size_t YComponent = 1;
			static constexpr size_t ZComponent = 2;

		private:
			float Components[3] = { 0.0f, 0.0f, 0.0f };

		public:
			// Constructors.
			Vector3() = default;
			Vector3(float x, float y, float z);

			// Static vector3 math operations.
			// Computes the dot product between a and b. Both a and b must be of unit length 1 (normalized).
			static float DotProduct(const Vector3& a, const Vector3& b);
			// Computes the vector orthogonal to a and b.
			static Vector3 CrossProduct(const Vector3& a, const Vector3& b);

			// In-place math operations.
			// Converts the vector to keep the same direction with unit length 1.
			void Normalize();

			// Getter/setters.
			inline float X() const { return Components[XComponent]; }
			inline float Y() const { return Components[YComponent]; }
			inline float Z() const { return Components[ZComponent]; }
			inline float X(const float x) { Components[XComponent] = x; }
			inline float Y(const float y) { Components[YComponent] = y; }
			inline float Z(const float z) { Components[ZComponent] = z; }

			// Returns the vector pointing in the same direction with unit length 1.
			Vector3 AsNormalized() const;

			// Returns the length (magnitude) of the vector.
			float Length() const;
		};

		class Matrix4x4
		{
		private:
			float Matrix[4 * 4] =
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

		public:
			// Constructors.
			Matrix4x4() = default;

			// Returns an identity matrix.
			static Matrix4x4 Identity();
		};
	}
}