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
		bool Shutdown();
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

		// Releases the platform cursor from the platform window it is currently locked to.
		bool ReleasePlatformCursor();

#ifdef LEVIATHAN_WITH_TOOLS
		bool ImGuiPlatformInitialize(void* platformWindowHandle);
		void ImGuiPlatformShutdown();
		void ImGuiPlatformNewFrame();
#endif // LEVIATHAN_WITH_TOOLS.
	}
}