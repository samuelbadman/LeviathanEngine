#pragma once

namespace LeviathanCore
{
	namespace MathTypes
	{
		class Vector2
		{
		private:
			static constexpr size_t XComponent = 0;
			static constexpr size_t YComponent = 1;

		private:
			float Components[2] = { 0.0f, 0.0f };

		public:
			Vector2() = default;
			constexpr Vector2(float x, float y)
				: Components{ x, y }
			{
			}

			inline float GetX() const { return Components[XComponent]; }
			inline float GetY() const { return Components[YComponent]; }
			void SetX(const float x) { Components[XComponent] = x; }
			void SetY(const float y) { Components[YComponent] = y; }

			inline const float* Data() const { return Components; }
			inline float* Data() { return Components; }

			Vector2 operator-(const Vector2& rhs) const;
		};

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
			constexpr Vector3(float x, float y, float z)
				: Components{ x, y, z }
			{
			}

			// Static vector3 math operations.
			// Computes the dot product between a and b. Both a and b must be of unit length 1 (normalized).
			static float DotProduct(const Vector3& a, const Vector3& b);
			// Computes the vector orthogonal to a and b.
			static Vector3 CrossProduct(const Vector3& a, const Vector3& b);

			// In-place math operations.
			// Converts the vector to keep the same direction with unit length 1.
			void NormalizeSafe();

			// Getter/setters.
			inline float GetX() const { return Components[XComponent]; }
			inline float GetY() const { return Components[YComponent]; }
			inline float GetZ() const { return Components[ZComponent]; }
			inline void SetX(const float x) { Components[XComponent] = x; }
			inline void SetY(const float y) { Components[YComponent] = y; }
			inline void SetZ(const float z) { Components[ZComponent] = z; }

			inline const float* Data() const { return Components; }
			inline float* Data() { return Components; }

			// Returns the vector pointing in the same direction with unit length 1.
			Vector3 AsNormalizedSafe() const;

			// Returns the length (magnitude) of the vector.
			float Length() const;
			// Returns the squared length (magnitude) of the vector.
			float SquaredLength() const;

			// Operators.
			Vector3 operator*(float rhs) const;
			Vector3 operator+(const Vector3& rhs) const;
			Vector3 operator-(const Vector3& rhs) const;
		};

		class Vector4
		{
		private:
			static constexpr size_t XComponent = 0;
			static constexpr size_t YComponent = 1;
			static constexpr size_t ZComponent = 2;
			static constexpr size_t WComponent = 3;

		private:
			float Components[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		public:
			Vector4() = default;
			Vector4(float x, float y, float z, float w);
			Vector4(const Vector3& xyz, float w);

			inline float GetX() const { return Components[XComponent]; }
			inline float GetY() const { return Components[YComponent]; }
			inline float GetZ() const { return Components[ZComponent]; }
			inline float GetW() const { return Components[WComponent]; }
			inline float SetX(float x) { Components[XComponent] = x; }
			inline float SetY(float y) { Components[YComponent] = y; }
			inline float SetZ(float z) { Components[ZComponent] = z; }
			inline float SetW(float w) { Components[WComponent] = w; }

			inline const float* Data() const { return Components; }
			inline float* Data() { return Components; }
		};

		//class Matrix3x3
		//{
		//private:
		//	float Matrix[3 * 3] =
		//	{
		//		1.0f, 0.0f, 0.0f,
		//		0.0f, 1.0f, 0.0f,
		//		0.0f, 0.0f, 1.0f
		//	};

		//public:
		//	// Constructors.
		//	Matrix3x3() = default;
		//	Matrix3x3(float e00, float e10, float e20,
		//		float e01, float e11, float e21,
		//		float e02, float e12, float e22);

		//	// Returns an identity 3x3 matrix.
		//	static Matrix3x3 Identity();
		//};

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
			Matrix4x4(float e00, float e10, float e20, float e30,
				float e01, float e11, float e21, float e31,
				float e02, float e12, float e22, float e32,
				float e03, float e13, float e23, float e33);

			// Returns an identity 4x4 matrix.
			static Matrix4x4 Identity();

			// Returns the transposed input matrix.
			static Matrix4x4 Transpose(const Matrix4x4& matrix4x4);

			// Returns the inverse 4x4 matrix of the input matrix.
			static Matrix4x4 Inverse(const Matrix4x4& matrix4x4);

			// Returns a 4x4 translation matrix.
			static Matrix4x4 Translation(const Vector3& translation);

			// Returns a 4x4 scale matrix.
			static Matrix4x4 Scaling(const Vector3& scale);

			// Returns a 4x4 rotation matrix from axis and angle in radians.
			static Matrix4x4 Rotation(const Vector3& axis, const float angleRadians);

			// Returns a 4x4 rotation matrix from euler angles. RotationRadians is applied roll first, then pitch and then yaw.
			static Matrix4x4 Rotation(const class Euler& euler);

			// Returns a 4x4 rotation matrix fom a quaternion.
			static Matrix4x4 Rotation(const class Quaternion& quaternion);

			// Returns a 4x4 view matrix.
			static Matrix4x4 View(const Vector3& cameraTranslation, const Euler& cameraRotation);

			// Returns a 4x4 perspective projection matrix.
			static Matrix4x4 PerspectiveProjection(float fovVerticalRadians, float aspectRatio, float nearZ, float farZ);

			// Returns a 4x4 orthographic projection matrix.
			static Matrix4x4 OrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ);

			// In-place math operations.
			void TransposeInPlace();
			void InverseInPlace();

			// Getters/setters.
			inline float* Data() { return Matrix; }
			inline const float* Data() const { return Matrix; }

			// Operators.
			Matrix4x4 operator*(const Matrix4x4& rhs) const;
			void operator*=(const Matrix4x4& rhs);
			Vector4 operator*(const Vector4& rhs) const;

		private:
			// Returns the result matrix of matrix a multiplied with matrix b.
			static Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b);
			// Returns the result vector of matrix a multiplied with vector4 b.
			static Vector4 Multiply(const Matrix4x4& a, const Vector4& b);
		};

		class Euler
		{
		private:
			static constexpr size_t PitchComponent = 0;
			static constexpr size_t YawComponent = 1;
			static constexpr size_t RollComponent = 2;

		private:
			// Rotation angles in the order pitch, yaw and roll in radians.
			float RotationRadians[3] = { 0.0f, 0.0f, 0.0f };

		public:
			// Constructors.
			Euler() = default;
			Euler(float pitchRadians, float yawRadians, float rollRadians);

			// Getters/setters.
			inline float GetPitchRadians() const { return RotationRadians[PitchComponent]; }
			inline float GetYawRadians() const { return RotationRadians[YawComponent]; }
			inline float GetRollRadians() const { return RotationRadians[RollComponent]; }
			inline void SetPitchRadians(const float pitchRadians) { RotationRadians[PitchComponent] = pitchRadians; }
			inline void SetYawRadians(const float yawRadians) { RotationRadians[YawComponent] = yawRadians; }
			inline void SetRollRadians(const float rollRadians) { RotationRadians[RollComponent] = rollRadians; }
		};

		class Quaternion
		{
		private:
			static constexpr size_t XComponent = 0;
			static constexpr size_t YComponent = 1;
			static constexpr size_t ZComponent = 2;
			static constexpr size_t WComponent = 3;

		private:
			float Components[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		public:
			// Constructors.
			Quaternion() = default;
			Quaternion(float x, float y, float z, float w);
			Quaternion(const Euler& euler);

			// Returns the identity quaternion.
			static inline Quaternion Identity() { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }

			// Returns the euler angles represented as a quaternion.
			static Quaternion MakeFromEuler(const Euler& euler);

			// Getters/setters.
			inline float GetX() const { return Components[XComponent]; }
			inline float GetY() const { return Components[YComponent]; }
			inline float GetZ() const { return Components[ZComponent]; }
			inline float GetW() const { return Components[WComponent]; }
			inline void SetX(const float x) { Components[XComponent] = x; }
			inline void SetY(const float y) { Components[YComponent] = y; }
			inline void SetZ(const float z) { Components[ZComponent] = z; }
			inline void SetW(const float w) { Components[WComponent] = w; }

			// Operators.
			Vector3 operator*(const Vector3& rhs);
		};
	}
}