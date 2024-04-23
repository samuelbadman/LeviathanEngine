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

bool AssetImporter::ImportModel(std::string_view file, AssetImporter::AssetTypes::Model& outModel)
{
	outModel = {};
	return ModelImporter::LoadModel(file, outModel);
}
