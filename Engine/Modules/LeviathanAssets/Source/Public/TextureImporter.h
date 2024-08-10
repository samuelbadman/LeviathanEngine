#pragma once

namespace LeviathanAssets
{
	namespace AssetTypes
	{
		struct Texture;
		struct HDRTexture;
	}

	namespace TextureImporter
	{
		bool LoadTexture(std::string_view filename, AssetTypes::Texture& outTexture);
		bool LoadHDRTexture(std::string_view filename, AssetTypes::HDRTexture& outHDRTexture);
		void FreeTexture(void* const data);
	}
}