#pragma once
#include "RendererConstants.h"
#include "LightTypes.h"

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

		struct DirectionalLightConstantBuffer
		{
			float Radiance[3] = { 0.0f };
			char Padding0[4] = { 0 };

			float LightDirectionViewSpace[3] = { 0.0f };
			char Padding1[4] = { 0 };
		};

		struct PointLightConstantBuffer
		{
			float Radiance[3] = { 0.0f };
			char Padding0[4] = { 0 };

			float LightPositionViewSpace[3] = { 0.0f };
			char Padding1[4] = { 0 };
		};

		struct SpotLightConstantBuffer
		{
			float Radiance[3] = { 0.0f };
			char Padding0[4] = { 0 };

			float LightPositionViewSpace[3] = { 0.0f };
			char Padding1[4] = { 0 };

			float LightDirectionViewSpace[3] = { 0.0f };
			float CosineInnerConeAngle = 0.0f;

			float CosineOuterConeAngle = 0.0f;
			char Padding2[4 * 3] = { 0 };
		};

		struct EquirectangularToCubemapConstantBuffer
		{
			float ViewProjectionMatrix[4 * 4] =
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		};

		struct SkyboxConstantBuffer
		{
			float ViewProjectionMatrix[4 * 4] =
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		};
	}
}