#pragma once

namespace LeviathanCore
{
	namespace Platform
	{
		bool CreateDebugConsole();
		bool DestroyDebugConsole();
		bool InitializeTiming();
		bool UpdateDeltaTime();
		float GetDeltaTimeInMilliseconds();
		float GetDeltaTimeInSeconds();
	}
}