#pragma once

namespace LeviathanRenderer
{
	namespace ConstantBufferTypes
	{
		// Constant buffer attributes in HLSL must be 16 byte aligned.

		struct SceneConstantBuffer
		{
			// 16 bytes.
			float Light[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

			// 16 bytes.
			float LightPositionViewSpace[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		};

		struct ObjectConstantBuffer
		{
			// 64 bytes.
			float WorldViewMatrix[4 * 4] =
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

			// 64 bytes.
			float WorldViewProjectionMatrix[4 * 4] =
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

			// 64 bytes.
			float NormalMatrix[4 * 4] =
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		};

		struct MaterialConstantBuffer
		{
			// 16 bytes.
			float Color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		};
	}
}