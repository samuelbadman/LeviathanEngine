#pragma once

namespace LeviathanRenderer
{
	namespace ConstantBufferTypes
	{
		struct SceneConstantBuffer
		{
			float DirectionalLightRadiance[8] = { 0.0f };
			float LightDirectionViewSpace[8] = { 0.0f };
		};

		struct ObjectConstantBuffer
		{
			float WorldViewMatrix[4 * 4] =
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

			float WorldViewProjectionMatrix[4 * 4] =
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

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
			float Color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		};
	}
}