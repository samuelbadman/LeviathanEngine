#pragma once

namespace LeviathanCore
{
	namespace MathTypes
	{
		class Vector3;
		class Vector2;
	}
}

namespace AssetImporter
{
	namespace ModelImporter
	{
		bool LoadModel(std::string_view filepath, 
			std::vector<LeviathanCore::MathTypes::Vector3>& outPositions,
			std::vector<LeviathanCore::MathTypes::Vector3>& outNormals,
			std::vector<LeviathanCore::MathTypes::Vector2>& outTextureCoordinates,
			std::vector<uint32_t>& outIndices);
	}
}