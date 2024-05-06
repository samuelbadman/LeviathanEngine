#pragma once

namespace LeviathanAssets
{
	namespace AssetTypes
	{
		struct Mesh;
	}

	namespace ModelImporter
	{
		// Returns true if the model loaded succesfully, false if the model failed to load and returns an array containing all of the meshes in the file in outMeshses.
		bool LoadModel(std::string_view file, std::vector<AssetTypes::Mesh>& outMeshes);

		// Returns all of the input meshes combined into a single mesh.
		AssetTypes::Mesh CombineMeshes(const AssetTypes::Mesh* const meshes, const size_t count);

		AssetTypes::Mesh GeneratePlanePrimitive(float halfWidth);
		AssetTypes::Mesh GenerateCubePrimitive(float halfWidth);
		AssetTypes::Mesh GenerateSpherePrimitive(float radius, int32_t sectors, int32_t stacks);
		AssetTypes::Mesh GenerateCylinderPrimitive(float baseRadius, float topRadius, float height, int32_t sectors, int32_t stacks);
		AssetTypes::Mesh GenerateConePrimitive(float baseRadius, float height, int32_t sectors, int32_t stacks);
	}
}