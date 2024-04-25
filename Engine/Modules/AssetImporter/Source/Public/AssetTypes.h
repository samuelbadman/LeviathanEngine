#pragma once

#include "MathTypes.h"

namespace AssetImporter
{
	namespace AssetTypes
	{
		struct Mesh
		{
			std::vector<LeviathanCore::MathTypes::Vector3> Positions = {};
			std::vector<LeviathanCore::MathTypes::Vector3> Normals = {};
			std::vector<LeviathanCore::MathTypes::Vector2> TextureCoordinates = {};
			std::vector<uint32_t> Indices = {};
		};
	}
}