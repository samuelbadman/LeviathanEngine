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

		// Returns all of the input meshes combined into a single mesh.
		AssetImporter::AssetTypes::Mesh CombineMeshes(const AssetImporter::AssetTypes::Mesh* const meshes, const size_t count);
	}
}