#include "AssetImporter.h"
#include "ModelImporter.h"

bool AssetImporter::Initialize()
{
	return true;
}

bool AssetImporter::Shutdown()
{
	return true;
}

bool AssetImporter::ImportModel(std::string_view file, AssetImporter::Mesh& outMesh)
{
	outMesh = {};
	return ModelImporter::LoadModel(file, outMesh.positions, outMesh.normals, outMesh.textureCoordinates, outMesh.indices);
}
