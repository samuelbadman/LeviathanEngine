#pragma once

namespace LeviathanRenderer
{
	struct LinearColor
	{
		// Four 8 bit integers packed into a single 32 bit integer value. Each 8 bits stores a 0-255 color value for a color channel. ABGR/RGBA depending on device endianness.
		uint32_t ColorValue = 0xffffffff;

		LinearColor() = default;
		LinearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		// Returns an RGB value in the range 0 - 255. Expects an RGB value in the range 0 - 1.
		static float ConvertUnitRGBToRGB(const float unitRGB);
		// Returns an RGB value in the range 0 - 1. Expects an RGB value in the range 0 - 255.
		static float ConvertRGBToUnitRGB(const float RGB);
		void SwapRedBlueChannels();
	};
}