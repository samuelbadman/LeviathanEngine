#pragma once

namespace LeviathanRenderer
{
	namespace RendererConstants
	{
		static constexpr size_t MaxDirectionalLightCount = 10;
		static constexpr size_t MaxPointLightCount = 10;
		static constexpr size_t MaxSpotLightCount = 10;

		static constexpr size_t Texture2DSRVTableLength = 4;
		static constexpr size_t TextureSamplerTableLength = 4;

		static constexpr size_t ColorTexture2DSRVTableIndex = 0;
		static constexpr size_t RoughnessTexture2DSRVTableIndex = 1;
		static constexpr size_t MetallicTexture2DSRVTableIndex = 2;
		static constexpr size_t NormalTexture2DSRVTableIndex = 3;

		static constexpr size_t ColorTextureSamplerTableIndex = 0;
		static constexpr size_t RoughnessTextureSamplerTableIndex = 1;
		static constexpr size_t MetallicTextureSamplerTableIndex = 2;
		static constexpr size_t NormalTextureSamplerTableIndex = 3;
	}
}