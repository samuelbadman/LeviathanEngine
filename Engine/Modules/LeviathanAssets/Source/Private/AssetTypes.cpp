#include "AssetTypes.h"

void LeviathanAssets::AssetTypes::Texture::FlipGreenChannel()
{
	const size_t pixelCount = static_cast<size_t>(Width * Height);
	for (size_t i = 0; i < pixelCount; ++i)
	{
		// Every pixel has Num8BitComponentsPerPixel 8 bit components (color channels in rgba format).
		unsigned char* const pixel = Data + (i * Num8BitComponentsPerPixel);
		unsigned char& greenChannel = *(pixel + 1);
		greenChannel = 255 - greenChannel;
	}
}
