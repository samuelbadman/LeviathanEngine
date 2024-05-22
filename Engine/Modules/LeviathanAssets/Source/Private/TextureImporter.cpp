#include "TextureImporter.h"

// Stb.
#define STB_IMAGE_IMPLEMENTATION
#include "Stb/stb_image.h"

#include "AssetTypes.h"

bool LeviathanAssets::TextureImporter::LoadTexture(std::string_view filename, AssetTypes::Texture& outTexture)
{
	outTexture = {};
	outTexture.Data = stbi_load(filename.data(), &outTexture.Width, &outTexture.Height, &outTexture.NumComponents, 0);
	return (outTexture.Data != nullptr);
}
