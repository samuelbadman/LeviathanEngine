#include "MathTypes.h"

namespace LeviathanCore
{
	namespace MathTypes
	{
		static DirectX::XMVECTOR XMVectorFromVector3(const Vector3& vector3)
		{
			return DirectX::XMVectorSet(vector3.X(), vector3.Y(), vector3.Z(), 1.0f);
		}

		static Vector3 Vector3FromXMVector(const DirectX::XMVECTOR& xmvector)
		{
			DirectX::XMFLOAT3 float3 = {};
			DirectX::XMStoreFloat3(&float3, xmvector);
			return Vector3(float3.x, float3.y, float3.z);
		}

		static Vector3 NormalizeVector3(const Vector3& vector3)
		{
			const DirectX::XMVECTOR vector = XMVectorFromVector3(vector3);
			const DirectX::XMVECTOR resultVector = DirectX::XMVector3Normalize(vector);
			return Vector3FromXMVector(resultVector);
		}

		Vector3::Vector3(float x, float y, float z)
			: Components{ x, y, z }
		{
		}

		float Vector3::DotProduct(const Vector3& a, const Vector3& b)
		{
			const DirectX::XMVECTOR vectorA = XMVectorFromVector3(a);
			const DirectX::XMVECTOR vectorB = XMVectorFromVector3(b);

			const DirectX::XMVECTOR resultVector = DirectX::XMVector3Dot(vectorA, vectorB);

			float result = 0.0f;
			DirectX::XMStoreFloat(&result, resultVector);

			return result;
		}

		Vector3 Vector3::CrossProduct(const Vector3& a, const Vector3& b)
		{
			const DirectX::XMVECTOR vectorA = XMVectorFromVector3(a);
			const DirectX::XMVECTOR vectorB = XMVectorFromVector3(b);

			const DirectX::XMVECTOR resultVector = DirectX::XMVector3Cross(vectorA, vectorB);

			return Vector3FromXMVector(resultVector);
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
			const DirectX::XMVECTOR vector = XMVectorFromVector3(*this);

			const DirectX::XMVECTOR resultVector = DirectX::XMVector3Length(vector);

			float result = 0.0f;
			DirectX::XMStoreFloat(&result, resultVector);

			return result;
		}
	}
}