#pragma once

namespace LeviathanRenderer
{
	namespace VertexTypes
	{
		struct VertexPos3
		{
			float Position[3] = { 0.0f, 0.0f, 0.0f };
		};

		struct VertexPos2UV2
		{
			float Position[2] = { 0.0f, 0.0f };
			float UV[2] = { 0.0f, 0.0f };
		};

		struct VertexPos3Norm3UV2Tang3
		{
			float Position[3] = { 0.0f, 0.0f, 0.0f };
			float Normal[3] = { 0.0f, 0.0f, 0.0f };
			float UV[2] = { 0.0f, 0.0f };
			float Tangent[3] = { 0.0f, 0.0f, 0.0f };
		};
	}
}