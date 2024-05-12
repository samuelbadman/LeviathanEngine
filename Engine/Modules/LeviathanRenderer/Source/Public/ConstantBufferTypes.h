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
				float Constant = 0.0f;
				float Linear = 0.0f;
				float Quadratic = 0.0f;
				char Padding0[4] = { 0 };

				float Radiance[3] = { 0.0f };
				char Padding1[4] = { 0 };

				float PositionViewSpace[3] = { 0.0f };
				char Padding2[4] = { 0 };
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
				float Constant = 0.0f;
				float Linear = 0.0f;
				float Quadratic = 0.0f;
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

		struct SceneConstantBuffer
		{
			uint32_t DirectionalLightCount = 0;
			uint32_t PointLightCount = 0;
			uint32_t SpotLightCount = 0;
			char Padding0[4] = { 0 };

			LightTypes::DirectionalLight DirectionalLights[RendererConstants::MaxDirectionalLightCount] = {};
			LightTypes::PointLight PointLights[RendererConstants::MaxPointLightCount] = {};
			LightTypes::SpotLight SpotLights[RendererConstants::MaxSpotLightCount] = {};
		};

		struct MaterialConstantBuffer
		{
			float Color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		};
	}
}