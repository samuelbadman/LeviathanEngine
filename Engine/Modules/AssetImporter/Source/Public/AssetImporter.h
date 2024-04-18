#pragma once

namespace AssetImporter
{
	bool Initialize();
	bool Shutdown();
	bool ImportModel(std::string_view file);
}