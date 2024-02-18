#pragma once

namespace LeviathanRenderer
{
	namespace ConstantBufferTypes
	{
		// Constant buffers in HLSL must by 16 byte aligned.

		struct MaterialConstantBuffer
		{
			float Color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		};
	}
}