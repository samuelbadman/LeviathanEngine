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
				LeviathanCore::MathTypes::Vector3 Radiance{ 0.0f, 0.0f, 0.0f };
				float Padding0 = 0.0f;

				LeviathanCore::MathTypes::Vector3 DirectionViewSpace{ 0.0f, 0.0f, 0.0f };
				float Padding1 = 0.0f;
			};

			struct PointLight
			{
				float Constant = 0.0f;
				float Linear = 0.0f;
				float Quadratic = 0.0f;
				float Padding0 = 0.0f;

				LeviathanCore::MathTypes::Vector3 Radiance{ 0.0f, 0.0f, 0.0f };
				float Padding1 = 0.0f;

				LeviathanCore::MathTypes::Vector3 PositionViewSpace{ 0.0f, 0.0f, 0.0f };
				float Padding2 = 0.0f;
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
			uint32_t Padding[2] = { 0 };

			LightTypes::DirectionalLight DirectionalLights[RendererConstants::MaxDirectionalLightCount] = {};

			LightTypes::PointLight PointLights[RendererConstants::MaxPointLightCount] = {};
		};

		struct MaterialConstantBuffer
		{
			float Color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		};
	}
}