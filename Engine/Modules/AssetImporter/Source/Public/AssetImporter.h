#pragma once

namespace AssetImporter
{
	namespace AssetTypes
	{
		struct Mesh;
	}

	bool Initialize();
	bool Shutdown();
	bool ImportModel(std::string_view file, std::vector<AssetImporter::AssetTypes::Mesh>& outModel);
	AssetImporter::AssetTypes::Mesh CombineMeshes(const AssetImporter::AssetTypes::Mesh* const meshes, const size_t count);
	AssetImporter::AssetTypes::Mesh GeneratePlanePrimitiveModel(float planeWidth);
}