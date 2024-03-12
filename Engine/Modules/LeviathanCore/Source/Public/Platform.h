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
				int TopLeftX = 0;
				int TopLeftY = 0;
				unsigned int Width = 0;
				unsigned int Height = 0;
				unsigned int VerticalRefreshRateHertz = 0;
				std::string Name = {};
				std::string AdapterName = {};
			};

			int GetDisplayCount();
			DisplayDetails GetDisplayDetails(const int displayIndex);
		}

		void ShowPlatformCursor(const bool show);
		bool IsPlatformCursorVisible();
	}
}