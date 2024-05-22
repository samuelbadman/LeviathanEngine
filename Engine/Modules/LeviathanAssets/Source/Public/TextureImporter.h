#pragma once

namespace LeviathanAssets
{
	namespace AssetTypes
	{
		struct Texture;
	}

	namespace TextureImporter
	{
		bool LoadTexture(std::string_view filename, AssetTypes::Texture& outTexture);
	}
}