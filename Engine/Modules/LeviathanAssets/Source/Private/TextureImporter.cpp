#include "TextureImporter.h"

// Stb.
#define STB_IMAGE_IMPLEMENTATION
#include "Stb/stb_image.h"

#include "AssetTypes.h"

bool LeviathanAssets::TextureImporter::LoadTexture(std::string_view filename, AssetTypes::Texture& outTexture)
{
	outTexture = {};
	stbi_set_flip_vertically_on_load(1);
	// Use STBI_rgb_alpha as the fifth parameter to load any image padded to 4 channel rgba.
	// TODO: Investigate texture compression formats (BC5/BC7) to reduce unnecessary texture memory.
	outTexture.Data = stbi_load(filename.data(), &outTexture.Width, &outTexture.Height, &outTexture.NumComponents, 0);
	return (outTexture.Data != nullptr);
}
