#include "AssetImporter.h"
#include "ModelImporter.h"
#include "AssetTypes.h"

bool AssetImporter::Initialize()
{
	return true;
}

bool AssetImporter::Shutdown()
{
	return true;
}

bool AssetImporter::ImportModel(std::string_view file, std::vector<AssetImporter::AssetTypes::Mesh>& outMeshes)
{
	outMeshes.clear();
	return ModelImporter::LoadModel(file, outMeshes);
}

AssetImporter::AssetTypes::Mesh AssetImporter::CombineMeshes(const AssetImporter::AssetTypes::Mesh* const meshes, const size_t count)
{
	return ModelImporter::CombineMeshes(meshes, count);
}

AssetImporter::AssetTypes::Mesh AssetImporter::GeneratePlanePrimitiveModel(float planeHalfWidth)
{
	return ModelImporter::GeneratePlanePrimitive(planeHalfWidth);
}

AssetImporter::AssetTypes::Mesh AssetImporter::GenerateCubePrimitiveModel(float cubeHalfWidth)
{
	return ModelImporter::GenerateCubePrimitive(cubeHalfWidth);
}

AssetImporter::AssetTypes::Mesh AssetImporter::GenerateSpherePrimitiveModel(float sphereRadius, int32_t sphereSectors, int32_t sphereStacks)
{
	return ModelImporter::GenerateSpherePrimitive(sphereRadius, sphereSectors, sphereStacks);
}

AssetImporter::AssetTypes::Mesh AssetImporter::GenerateCylinderPrimitiveModel(float cylinderBaseRadius, float cylinderTopRadius, float cylinderHeight, int32_t cylinderSectors, 
	int32_t cylinderStacks)
{
	return ModelImporter::GenerateCylinderPrimitive(cylinderBaseRadius, cylinderTopRadius, cylinderHeight, cylinderSectors, cylinderStacks);
}

AssetImporter::AssetTypes::Mesh AssetImporter::GenerateConePrimitiveModel(float coneBaseRadius, float coneHeight, int32_t coneSectors, int32_t coneStacks)
{
	return ModelImporter::GenerateConePrimitive(coneBaseRadius, coneHeight, coneSectors, coneStacks);
}
