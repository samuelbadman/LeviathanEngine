#pragma once
#include "RendererConstants.h"

namespace LeviathanRenderer
{
	namespace ConstantBufferTypes
	{
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

		struct SceneConstantBuffer
		{
			uint32_t DirectionalLightCount = 0;
			uint32_t PointLightCount = 0;
			uint32_t Padding[2] = { 0 };

			float DirectionalLightRadiance[4 * RendererConstants::MaxDirectionalLightCount] = { 0.0f };
			float LightDirectionViewSpace[4 * RendererConstants::MaxDirectionalLightCount] = { 0.0f };

			float PointLightRadiance[4 * RendererConstants::MaxPointLightCount] = { 0.0f };
			float PointLightPositionViewSpace[4 * RendererConstants::MaxPointLightCount] = { 0.0f };
		};

		struct MaterialConstantBuffer
		{
			float Color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		};
	}
}