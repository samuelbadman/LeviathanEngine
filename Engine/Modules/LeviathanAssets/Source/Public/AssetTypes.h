#pragma once

#include "MathTypes.h"

namespace LeviathanAssets
{
	namespace AssetTypes
	{
		struct Mesh
		{
			std::vector<LeviathanCore::MathTypes::Vector3> Positions = {};
			std::vector<LeviathanCore::MathTypes::Vector3> Normals = {};
			std::vector<LeviathanCore::MathTypes::Vector2> TextureCoordinates = {};
			std::vector<LeviathanCore::MathTypes::Vector3> Tangents = {};
			std::vector<uint32_t> Indices = {};
		};

		struct Texture
		{
			int Width = 0;
			int Height = 0;
			int NumComponents = 0;
			unsigned char* Data = nullptr;
		};
	}
}