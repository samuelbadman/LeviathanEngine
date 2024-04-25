#pragma once

namespace AssetImporter
{
	namespace AssetTypes
	{
		struct Mesh;
	}

	namespace ModelImporter
	{
		// Returns true if the model loaded succesfully, false if the model failed to load and returns an array containing all of the meshes in the file in outMeshses.
		bool LoadModel(std::string_view file, std::vector<AssetImporter::AssetTypes::Mesh>& outMeshes);
	}
}