#pragma once

#include "Callback.h"

namespace LeviathanCore
{
	namespace Platform
	{
		using GameControllerConnectionEventCallbackType = void(*)();

		extern Callback<GameControllerConnectionEventCallbackType> GameControllerConnectionEventCallback;

		bool CreateDebugConsole();
		bool DestroyDebugConsole();
		bool Initialize();
		bool TickPlatform();
		float GetDeltaTimeInMilliseconds();
		float GetDeltaTimeInSeconds();
	}
}