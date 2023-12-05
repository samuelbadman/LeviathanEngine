#pragma once

#include "Callback.h"

namespace LeviathanCore
{
	namespace Platform
	{
		using GameControllerConnectionEventCallbackType = void(*)();

		bool CreateDebugConsole();
		bool DestroyDebugConsole();
		bool Initialize();
		bool TickPlatform();
		float GetDeltaTimeInMilliseconds();
		float GetDeltaTimeInSeconds();
		Callback<GameControllerConnectionEventCallbackType>& GetGameControllerConnectionEventCallback();

		namespace Displays
		{
			struct DisplayDetails
			{
				int32_t TopLeftX = 0;
				int32_t TopLeftY = 0;
				uint32_t Width = 0;
				uint32_t Height = 0;
				uint32_t VerticalRefreshRateHertz = 0;
				std::string Name = {};
				std::string AdapterName = {};
			};

			int GetDisplayCount();
			DisplayDetails GetDisplayDetails(const int displayIndex);
		}
	}
}