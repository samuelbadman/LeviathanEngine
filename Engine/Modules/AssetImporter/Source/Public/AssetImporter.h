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
}