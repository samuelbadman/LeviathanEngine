#include "LinearColor.h"
#include "MathLibrary.h"

LeviathanRenderer::LinearColor::LinearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	// Color value is stored in ABGR (alpha, blue, green, red) format by default.
	ColorValue =
		((a & 0xff) << 24) |
		((b & 0xff) << 16) |
		((g & 0xff) << 8) |
		((r & 0xff));
}

float LeviathanRenderer::LinearColor::ConvertUnitRGBToRGB(const float unitRGB)
{
	return LeviathanCore::MathLibrary::Clamp(unitRGB * 255.0f, 0.0f, 255.0f);
}

float LeviathanRenderer::LinearColor::ConvertRGBToUnitRGB(const float RGB)
{
	return LeviathanCore::MathLibrary::Clamp(RGB / 255.0f, 0.0f, 1.0f);
}

void LeviathanRenderer::LinearColor::SwapRedBlueChannels()
{
	const uint8_t temp = static_cast<uint8_t>((ColorValue & 0x00ff0000) >> 16); // Copy red channel.
	ColorValue = (ColorValue & 0xff00ffff) | ((static_cast<uint8_t>(ColorValue & 0x000000ff) & 0xff) << 16); // Set red channel to blue channel.
	ColorValue = (ColorValue & 0xffffff00) | ((temp & 0xff)); // Set blue channel to red channel.
}
