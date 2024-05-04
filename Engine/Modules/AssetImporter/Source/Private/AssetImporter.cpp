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

AssetImporter::AssetTypes::Mesh AssetImporter::GeneratePlanePrimitiveModel(float planeWidth)
{
	return ModelImporter::GeneratePlanePrimitive(planeWidth);
}
