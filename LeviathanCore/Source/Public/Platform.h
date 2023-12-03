#pragma once

namespace LeviathanCore
{
	namespace Platform
	{
		bool CreateDebugConsole();
		bool DestroyDebugConsole();
		bool Initialize();
		bool UpdateDeltaTime();
		float GetDeltaTimeInMilliseconds();
		float GetDeltaTimeInSeconds();
	}
}