#include "MathTypes.h"

namespace LeviathanCore
{
	namespace MathTypes
	{
		static DirectX::XMVECTOR XMVECTORFromVector3(const Vector3& vector3)
		{
			return DirectX::XMVectorSet(vector3.GetX(), vector3.GetY(), vector3.GetZ(), 1.0f);
		}

		static Vector3 Vector3FromXMVECTOR(const DirectX::XMVECTOR& xmvector)
		{
			DirectX::XMFLOAT3 float3 = {};
			DirectX::XMStoreFloat3(&float3, xmvector);
			return Vector3(float3.x, float3.y, float3.z);
		}

		static Vector3 NormalizeVector3(const Vector3& vector3)
		{
			const DirectX::XMVECTOR vector = XMVECTORFromVector3(vector3);
			const DirectX::XMVECTOR resultVector = DirectX::XMVector3Normalize(vector);
			return Vector3FromXMVECTOR(resultVector);
		}

		static Matrix4x4 Matrix4x4FromXMMATRIX(const DirectX::XMMATRIX& xmmatrix)
		{
			DirectX::XMFLOAT4X4 float4x4 = {};
			DirectX::XMStoreFloat4x4(&float4x4, xmmatrix);
			Matrix4x4 result = {};
			memcpy(result.GetMatrix(), float4x4.m, sizeof(float) * 16);
			return result;
		}

		static DirectX::XMMATRIX XMMATRIXFromMatrix4x4(const Matrix4x4& matrix4x4)
		{
			const float* pMatrix = matrix4x4.GetMatrix();
			return DirectX::XMMatrixSet(pMatrix[0], pMatrix[1], pMatrix[2], pMatrix[3],
				pMatrix[4], pMatrix[5], pMatrix[6], pMatrix[7],
				pMatrix[8], pMatrix[9], pMatrix[10], pMatrix[11],
				pMatrix[12], pMatrix[13], pMatrix[14], pMatrix[15]);
		}

		static DirectX::XMVECTOR XMVECTORFromQuaternion(const Quaternion& quaternion)
		{
			const DirectX::XMVECTOR result = DirectX::XMVectorSet(quaternion.GetX(), quaternion.GetY(), quaternion.GetZ(), quaternion.GetW());
			return result;
		}

		static Quaternion QuaternionFromXMVECTOR(const DirectX::XMVECTOR& xmvector)
		{
			DirectX::XMFLOAT4 float4 = {};
			DirectX::XMStoreFloat4(&float4, xmvector);

			return Quaternion(float4.x, float4.y, float4.z, float4.w);
		}

		Vector3::Vector3(float x, float y, float z)
			: Components{ x, y, z }
		{
		}

		float Vector3::DotProduct(const Vector3& a, const Vector3& b)
		{
			const DirectX::XMVECTOR vectorA = XMVECTORFromVector3(a);
			const DirectX::XMVECTOR vectorB = XMVECTORFromVector3(b);

			const DirectX::XMVECTOR resultVector = DirectX::XMVector3Dot(vectorA, vectorB);

			float result = 0.0f;
			DirectX::XMStoreFloat(&result, resultVector);

			return result;
		}

		Vector3 Vector3::CrossProduct(const Vector3& a, const Vector3& b)
		{
			const DirectX::XMVECTOR vectorA = XMVECTORFromVector3(a);
			const DirectX::XMVECTOR vectorB = XMVECTORFromVector3(b);

			const DirectX::XMVECTOR resultVector = DirectX::XMVector3Cross(vectorA, vectorB);

			return Vector3FromXMVECTOR(resultVector);
		}

		void Vector3::Normalize()
		{
			*this = NormalizeVector3(*this);
		}

		Vector3 Vector3::AsNormalized() const
		{
			return NormalizeVector3(*this);
		}

		float Vector3::Length() const
		{
			const DirectX::XMVECTOR vector = XMVECTORFromVector3(*this);

			const DirectX::XMVECTOR resultVector = DirectX::XMVector3Length(vector);

			float result = 0.0f;
			DirectX::XMStoreFloat(&result, resultVector);

			return result;
		}

		Matrix4x4::Matrix4x4(float e00, float e10, float e20, float e30, float e01, float e11, float e21, float e31, float e02, float e12, float e22, float e32, float e03, float e13, float e23, float e33)
			: Matrix{  e00,  e10,  e20,  e30,  e01,  e11,  e21,  e31,  e02,  e12,  e22,  e32,  e03,  e13,  e23,  e33 }
		{
		}

		Matrix4x4 Matrix4x4::Identity()
		{
			return Matrix4x4();
		}

		Matrix4x4 Matrix4x4::Transpose(const Matrix4x4& matrix4x4)
		{
			return Matrix4x4FromXMMATRIX(DirectX::XMMatrixTranspose(XMMATRIXFromMatrix4x4(matrix4x4)));
		}

		Matrix4x4 Matrix4x4::Inverse(const Matrix4x4& matrix4x4)
		{
			return Matrix4x4FromXMMATRIX(DirectX::XMMatrixInverse(nullptr, XMMATRIXFromMatrix4x4(matrix4x4)));
		}

		Matrix4x4 Matrix4x4::Translation(const Vector3& translation)
		{
			return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				translation.GetX(), translation.GetY(), translation.GetZ(), 1.0f);
		}

		Matrix4x4 Matrix4x4::Scaling(const Vector3& scale)
		{
			return Matrix4x4(scale.GetX(), 0.0f, 0.0f, 0.0f,
				0.0f, scale.GetY(), 0.0f, 0.0f,
				0.0f, 0.0f, scale.GetZ(), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}

		Matrix4x4 Matrix4x4::Rotation(const Vector3& axis, const float angleRadians)
		{
			const DirectX::XMVECTOR axisVector = XMVECTORFromVector3(axis);
			const DirectX::XMMATRIX resultMatrix = DirectX::XMMatrixRotationAxis(axisVector, angleRadians);
			return Matrix4x4FromXMMATRIX(resultMatrix);
		}

		Matrix4x4 Matrix4x4::Rotation(const Euler& euler)
		{
			const DirectX::XMMATRIX resultMatrix = DirectX::XMMatrixRotationRollPitchYaw(euler.GetPitchRadians(), euler.GetYawRadians(), euler.GetRollRadians());
			return Matrix4x4FromXMMATRIX(resultMatrix);
		}

		Matrix4x4 Matrix4x4::Rotation(const Quaternion& quaternion)
		{
			const DirectX::XMVECTOR quaternionVector = XMVECTORFromQuaternion(quaternion);
			const DirectX::XMMATRIX resultMatrix = DirectX::XMMatrixRotationQuaternion(quaternionVector);
			return Matrix4x4FromXMMATRIX(resultMatrix);
		}

		Matrix4x4 Matrix4x4::View(const Vector3& cameraTranslation, [[maybe_unused]] const Euler& cameraRotation)
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
			Matrix4x4 ortho= {};
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
			const DirectX::XMMATRIX matA = XMMATRIXFromMatrix4x4(a);
			const DirectX::XMMATRIX matB = XMMATRIXFromMatrix4x4(b);
			const DirectX::XMMATRIX resultMatrix = DirectX::XMMatrixMultiply(matA, matB);
			return Matrix4x4FromXMMATRIX(resultMatrix);
		}

		Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs)
		{
			return Matrix4x4::Multiply(*this, rhs);
		}

		Euler::Euler(float pitchRadians, float yawRadians, float rollRadians)
			: Rotation{ pitchRadians, yawRadians, rollRadians }
		{
		}

		Quaternion::Quaternion(float x, float y, float z, float w)
			: Components{ x, y, z, w }
		{
		}

		Quaternion Quaternion::MakeFromEuler(const Euler& euler)
		{
			return QuaternionFromXMVECTOR(DirectX::XMQuaternionRotationRollPitchYaw(euler.GetPitchRadians(), euler.GetYawRadians(), euler.GetRollRadians()));
		}
	}
}