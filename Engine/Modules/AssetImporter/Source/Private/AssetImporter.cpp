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

bool AssetImporter::ImportModel(std::string_view file)
{
	return ModelImporter::LoadModel(file);
}
