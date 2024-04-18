#pragma once

namespace AssetImporter
{
	bool Initialize();
	bool Shutdown();
	bool LoadModel(const std::string& filepath);
}