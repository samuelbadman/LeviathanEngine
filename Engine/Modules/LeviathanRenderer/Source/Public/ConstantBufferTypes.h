#pragma once

namespace LeviathanRenderer
{
	namespace ConstantBufferTypes
	{
		// Constant buffers in HLSL must by 16 byte aligned.

		struct ObjectConstantBuffer
		{
			// 64 bytes.
			float WorldViewProjection[4 * 4] = 
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