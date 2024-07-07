#include "MathTypes.h"
#include "MathLibrary.h"

namespace LeviathanCore
{
	// TODO: Replace GLM math with custom math implementations.
	namespace MathTypes
	{
		Vector2 Vector2::operator-(const Vector2& rhs) const
		{
			return Vector2(X() - rhs.X(), Y() - rhs.Y());
		}

		float Vector3::DotProduct(const Vector3& a, const Vector3& b)
		{
			return (a.X() * b.X()) + (a.Y() * b.Y()) + (a.Z() * b.Z());
		}

		Vector3 Vector3::CrossProduct(const Vector3& a, const Vector3& b)
		{
			return Vector3((a.Y() * b.Z()) - (b.Y() * a.Z()), (b.X() * a.Z()) - (a.X() * b.Z()), (a.X() * b.Y()) - (b.X() * a.Y()));
		}

		void Vector3::NormalizeSafe()
		{
			const float length = Length();

			if (length == 0.0f)
			{
				return;
			}

			SetX(X() / length);
			SetY(Y() / length);
			SetZ(Z() / length);
		}

		Vector3 Vector3::AsNormalizedSafe() const
		{
			const float length = Length();

			if (length == 0.0f)
			{
				return Vector3();
			}

			return Vector3(X() / length, Y() / length, Z() / length);
		}

		float Vector3::Length() const
		{
			return sqrtf(MathLibrary::Square(X()) + MathLibrary::Square(Y()) + MathLibrary::Square(Z()));
		}

		float Vector3::SquaredLength() const
		{
			return (MathLibrary::Square(X()) + MathLibrary::Square(Y()) + MathLibrary::Square(Z()));
		}

		Vector3 Vector3::operator*(float rhs) const
		{
			return Vector3(X() * rhs, Y() * rhs, Z() * rhs);
		}

		Vector3 Vector3::operator+(const Vector3& rhs) const
		{
			return Vector3(X() + rhs.X(), Y() + rhs.Y(), Z() + rhs.Z());
		}

		Vector3 Vector3::operator-(const Vector3& rhs) const
		{
			return Vector3(X() - rhs.X(), Y() - rhs.Y(), Z() - rhs.Z());
		}

		Vector4::Vector4(float x, float y, float z, float w)
			: Components{ x, y, z, w }
		{
		}

		Vector4::Vector4(const Vector3& xyz, float w)
			:Components{ xyz.X(), xyz.Y(), xyz.Z(), w }
		{
		}

		//Matrix3x3::Matrix3x3(float e00, float e10, float e20, float e01, float e11, float e21, float e02, float e12, float e22)
		//	: Matrix{ e00, e10, e20, e01, e11, e21, e02, e12, e22 }
		//{
		//}

		//Matrix3x3 Matrix3x3::Identity()
		//{
		//	return Matrix3x3();
		//}

		Matrix4x4::Matrix4x4(float e00, float e10, float e20, float e30, float e01, float e11, float e21, float e31, float e02, float e12, float e22, float e32, float e03, float e13, float e23, float e33)
			: Matrix{ e00,  e10,  e20,  e30,  e01,  e11,  e21,  e31,  e02,  e12,  e22,  e32,  e03,  e13,  e23,  e33 }
		{
		}

		Matrix4x4 Matrix4x4::Identity()
		{
			return Matrix4x4();
		}

		Matrix4x4 Matrix4x4::Transpose(const Matrix4x4& matrix4x4)
		{
			glm::mat4x4 glmMat = {};
			memcpy(&glmMat[0], matrix4x4.Data(), sizeof(float) * 16);

			glmMat = glm::transpose(glmMat);

			Matrix4x4 result = {};
			memcpy(result.Data(), &glmMat[0], sizeof(float) * 16);

			return result;
		}

		Matrix4x4 Matrix4x4::Inverse(const Matrix4x4& matrix4x4)
		{
			glm::mat4x4 glmMat = {};
			memcpy(&glmMat[0], matrix4x4.Data(), sizeof(float) * 16);

			glmMat = glm::inverse(glmMat);

			Matrix4x4 result = {};
			memcpy(result.Data(), &glmMat[0], sizeof(float) * 16);

			return result;
		}

		Matrix4x4 Matrix4x4::Translation(const Vector3& translation)
		{
			const glm::mat4x4 glmMat = glm::translate(glm::identity<glm::mat4x4>(), glm::vec3(translation.X(), translation.Y(), translation.Z()));

			Matrix4x4 result = {};
			memcpy(result.Data(), &glmMat[0], sizeof(float) * 16);

			return result;
		}

		Matrix4x4 Matrix4x4::Scaling(const Vector3& scale)
		{
			const glm::mat4x4 glmMat = glm::scale(glm::identity<glm::mat4x4>(), glm::vec3(scale.X(), scale.Y(), scale.Z()));

			Matrix4x4 result = {};
			memcpy(result.Data(), &glmMat[0], sizeof(float) * 16);

			return result;
		}

		Matrix4x4 Matrix4x4::Rotation(const Vector3& axis, const float angleRadians)
		{
			const glm::mat4x4 rotation = glm::rotate(glm::identity<glm::mat4x4>(), angleRadians, glm::vec3(axis.X(), axis.Y(), axis.Z()));

			Matrix4x4 result = {};
			memcpy(result.Data(), &rotation[0], sizeof(float) * 16);

			return result;
		}

		Matrix4x4 Matrix4x4::Rotation(const Euler& euler)
		{
			const glm::mat4x4 rotation = glm::mat4_cast(glm::quat(glm::vec3(euler.PitchRadians(), euler.YawRadians(), euler.RollRadians())));

			Matrix4x4 result = {};
			memcpy(result.Data(), &rotation[0], sizeof(float) * 16);

			return result;
		}

		Matrix4x4 Matrix4x4::Rotation(const Quaternion& quaternion)
		{
			glm::mat4x4 rotation = glm::mat4_cast(glm::quat(quaternion.W(), quaternion.X(), quaternion.Y(), quaternion.Z()));

			Matrix4x4 result = {};
			memcpy(result.Data(), &rotation[0], sizeof(float) * 16);

			return result;
		}

		Matrix4x4 Matrix4x4::View(const Vector3& cameraTranslation, const Euler& cameraRotation)
		{
			const Matrix4x4 cameraTranslationRotationMatrix = (Matrix4x4::Translation(cameraTranslation) * Matrix4x4::Rotation(cameraRotation));
			return Matrix4x4::Inverse(cameraTranslationRotationMatrix);
		}

		Matrix4x4 Matrix4x4::PerspectiveProjection(float fovVerticalRadians, float aspectRatio, float nearZ, float farZ)
		{
			float h = 1.0f / tan(fovVerticalRadians * 0.5f);
			float w = h / aspectRatio;
			float a = farZ / (farZ - nearZ);
			float b = (-nearZ * farZ) / (farZ - nearZ);

			return Matrix4x4(w, 0.0f, 0.0f, 0.0f,
				0.0f, h, 0.0f, 0.0f,
				0.0f, 0.0f, a, 1.0f,
				0.0f, 0.0f, b, 0.0f);
		}

		Matrix4x4 Matrix4x4::OrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ)
		{
			Matrix4x4 ortho = {};
			ortho.Matrix[0] = 2.0f / viewWidth;
			ortho.Matrix[5] = 2.0f / viewHeight;
			ortho.Matrix[10] = 1.0f / (farZ - nearZ);
			ortho.Matrix[14] = -ortho.Matrix[10] * nearZ;
			ortho.Matrix[15] = 1.0f;

			return ortho;
		}

		void Matrix4x4::TransposeInPlace()
		{
			*this = Matrix4x4::Transpose(*this);
		}

		void Matrix4x4::InverseInPlace()
		{
			*this = Matrix4x4::Inverse(*this);
		}

		Matrix4x4 Matrix4x4::Multiply(const Matrix4x4& a, const Matrix4x4& b)
		{
			glm::mat4x4 glmMatA = {};
			memcpy(&glmMatA[0], a.Data(), sizeof(float) * 16);

			glm::mat4x4 glmMatB = {};
			memcpy(&glmMatB[0], b.Data(), sizeof(float) * 16);

			const glm::mat4x4 glmResult = glmMatA * glmMatB;

			Matrix4x4 result = {};
			memcpy(result.Data(), &glmResult[0], sizeof(float) * 16);

			return result;
		}

		Vector4 Matrix4x4::Multiply(const Matrix4x4& a, const Vector4& b)
		{
			glm::mat4x4 glmMatA = {};
			memcpy(&glmMatA[0], a.Data(), sizeof(float) * 16);

			glm::vec4 glmVec4{ b.X(), b.Y(), b.Z(), b.W() };

			const glm::vec4 glmResult = glmMatA * glmVec4;
			return Vector4(glmResult.x, glmResult.y, glmResult.z, glmResult.w);
		}

		Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) const
		{
			return Matrix4x4::Multiply(*this, rhs);
		}

		void Matrix4x4::operator*=(const Matrix4x4& rhs)
		{
			*this = Matrix4x4::Multiply(*this, rhs);
		}

		Vector4 Matrix4x4::operator*(const Vector4& rhs) const
		{
			return Matrix4x4::Multiply(*this, rhs);
		}

		Euler::Euler(float pitchRadians, float yawRadians, float rollRadians)
			: RotationRadians{ pitchRadians, yawRadians, rollRadians }
		{
		}

		Quaternion::Quaternion(float x, float y, float z, float w)
			: Components{ x, y, z, w }
		{
		}

		Quaternion::Quaternion(const Euler& euler)
		{
			*this = MakeFromEuler(euler);
		}

		Quaternion Quaternion::MakeFromEuler(const Euler& euler)
		{
			const glm::quat glmQuat = glm::quat(glm::vec3(euler.PitchRadians(), euler.YawRadians(), euler.RollRadians()));
			return Quaternion(glmQuat.x, glmQuat.y, glmQuat.z, glmQuat.w);
		}

		Vector3 Quaternion::operator*(const Vector3& rhs)
		{
			const glm::vec3 rotated = (glm::quat(Components[WComponent], Components[XComponent], Components[YComponent], Components[ZComponent]) * glm::vec3(rhs.X(), rhs.Y(), rhs.Z()));
			return Vector3(rotated.x, rotated.y, rotated.z);
		}
	}
}