#include "LinearColor.h"

LeviathanRenderer::LinearColor::LinearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	ColorValue =
		((a & 0xff) << 24) |
		((b & 0xff) << 16) |
		((g & 0xff) << 8) |
		((r & 0xff));
}

void LeviathanRenderer::LinearColor::SwapRedBlueChannels()
{
	const uint8_t temp = static_cast<uint8_t>((ColorValue & 0x00ff0000) >> 16); // Copy red channel.
	ColorValue = (ColorValue & 0xff00ffff) | ((static_cast<uint8_t>(ColorValue & 0x000000ff) & 0xff) << 16); // Set red channel to blue channel.
	ColorValue = (ColorValue & 0xffffff00) | ((temp & 0xff)); // Set blue channel to red channel.
}
