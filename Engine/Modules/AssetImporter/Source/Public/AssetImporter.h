#pragma once

#include "MathTypes.h"

namespace AssetImporter
{
	struct Mesh
	{
		std::vector<LeviathanCore::MathTypes::Vector3> positions = {};
		std::vector<LeviathanCore::MathTypes::Vector3> normals = {};
		std::vector<LeviathanCore::MathTypes::Vector2> textureCoordinates = {};
		std::vector<uint32_t> indices = {};
	};

	bool Initialize();
	bool Shutdown();
	bool ImportModel(std::string_view file, AssetImporter::Mesh& outMesh);
}