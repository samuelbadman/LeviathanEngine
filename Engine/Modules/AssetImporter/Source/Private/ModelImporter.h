#pragma once

namespace AssetImporter
{
	namespace AssetTypes
	{
		struct Model;
	}

	namespace ModelImporter
	{
		bool LoadModel(std::string_view filepath, AssetImporter::AssetTypes::Model& outModel);
	}
}