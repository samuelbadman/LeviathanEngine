#pragma once

namespace AssetImporter
{
	namespace AssetTypes
	{
		struct Model;
	}

	bool Initialize();
	bool Shutdown();
	bool ImportModel(std::string_view file, AssetImporter::AssetTypes::Model& outModel);
}