#pragma once
#include "RendererConstants.h"
#include "LightTypes.h"

namespace LeviathanRenderer
{
	namespace ConstantBufferTypes
	{
		namespace LightTypes
		{
			struct DirectionalLight
			{
				float Radiance[3] = { 0.0f };
				char Padding0[4] = { 0 };

				float DirectionViewSpace[3] = { 0.0f };
				char Padding1[4] = { 0 };
			};

			struct PointLight
			{
				float Radiance[3] = { 0.0f };
				char Padding0[4] = { 0 };

				float PositionViewSpace[3] = { 0.0f };
				char Padding1[4] = { 0 };
			};

			struct SpotLight
			{
				float Radiance[3] = { 0.0f };
				char Padding0[4] = { 0 };

				float PositionViewSpace[3] = { 0.0f };
				char Padding1[4] = { 0 };

				float DirectionViewSpace[3] = { 0.0f };
				float CosineInnerConeAngle = 0.0f;

				float CosineOuterConeAngle = 0.0f;
				char Padding2[4 * 3];
			};
		}

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

		struct DirectionalLightConstantBuffer
		{
			LightTypes::DirectionalLight DirectionalLight = {};
		};
	}
}