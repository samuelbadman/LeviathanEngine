#pragma once

namespace LeviathanRenderer
{
	namespace RendererConstants
	{
		static constexpr size_t Texture2DSRVTableLength = 4;
		static constexpr const char* Texture2DSRVTableLengthString = "4";
		static constexpr size_t TextureCubeSRVTableLength = 1;
		static constexpr const char* TextureCubeSRVTableLengthString = "1";
		static constexpr size_t TextureSamplerTableLength = 5;
		static constexpr const char* TextureSamplerTableLengthString = "5";

		static constexpr size_t EnvironmentTextureCubeSRVTableIndex = 0;
		static constexpr const char* EnvironmentTextureCubeSRVTableIndexString = "0";

		static constexpr size_t ColorTexture2DSRVTableIndex = 0;
		static constexpr const char* ColorTexture2DSRVTableIndexString = "0";
		static constexpr size_t RoughnessTexture2DSRVTableIndex = 1;
		static constexpr const char* RoughnessTexture2DSRVTableIndexString = "1";
		static constexpr size_t MetallicTexture2DSRVTableIndex = 2;
		static constexpr const char* MetallicTexture2DSRVTableIndexString = "2";
		static constexpr size_t NormalTexture2DSRVTableIndex = 3;
		static constexpr const char* NormalTexture2DSRVTableIndexString = "3";

		static constexpr size_t EnvironmentTextureSamplerTableIndex = 0;
		static constexpr const char* EnvironmentTextureSamplerTableIndexString = "0";
		static constexpr size_t ColorTextureSamplerTableIndex = 1;
		static constexpr const char* ColorTextureSamplerTableIndexString = "1";
		static constexpr size_t RoughnessTextureSamplerTableIndex = 2;
		static constexpr const char* RoughnessTextureSamplerTableIndexString = "2";
		static constexpr size_t MetallicTextureSamplerTableIndex = 3;
		static constexpr const char* MetallicTextureSamplerTableIndexString = "3";
		static constexpr size_t NormalTextureSamplerTableIndex = 4;
		static constexpr const char* NormalTextureSamplerTableIndexString = "4";
	}
}