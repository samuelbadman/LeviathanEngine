#pragma once

namespace LeviathanRenderer
{
	namespace VertexTypes
	{
		struct VertexPos
		{
			float Position[3] = { 0.0f, 0.0f, 0.0f };
		};

		struct VertexPosNormUVTang
		{
			float Position[3] = { 0.0f, 0.0f, 0.0f };
			float Normal[3] = { 0.0f, 0.0f, 0.0f };
			float UV[2] = { 0.0f, 0.0f };
			float Tangent[3] = { 0.0f, 0.0f, 0.0f };
		};
	}
}