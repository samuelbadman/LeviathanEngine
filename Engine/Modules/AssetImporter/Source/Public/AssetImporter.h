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
	AssetImporter::AssetTypes::Mesh GeneratePlanePrimitiveModel(float planeHalfWidth);
	AssetImporter::AssetTypes::Mesh GenerateCubePrimitiveModel(float cubeHalfWidth);
	AssetImporter::AssetTypes::Mesh GenerateSpherePrimitiveModel(float sphereRadius, int32_t sphereSectors, int32_t sphereStacks);
	AssetImporter::AssetTypes::Mesh GenerateCylinderPrimitiveModel(float cylinderBaseRadius, float cylinderTopRadius, float cylinderHeight, int32_t cylinderSectors, int32_t cylinderStacks);
	AssetImporter::AssetTypes::Mesh GenerateConePrimitiveModel(float coneBaseRadius, float coneHeight, int32_t coneSectors, int32_t coneStacks);
}