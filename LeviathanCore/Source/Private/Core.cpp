#include "Core.h"
#include "Platform.h"

namespace LeviathanCore
{
	namespace Core
	{
		Callback<PreMainLoopCallbackType> PreMainLoopCallback = {};
		Callback<PostMainLoopCallbackType> PostMainLoopCallback = {};
		Callback<TickCallbackType> TickCallback = {};

		static bool EngineRunning = false;

		void MainLoop()
		{
			while (EngineRunning)
			{
				LeviathanCore::Platform::UpdateDeltaTime();
				TickCallback.Call(LeviathanCore::Platform::GetDeltaTimeInSeconds());
			}
		}

		int RunEngine()
		{
			LeviathanCore::Platform::CreateDebugConsole();
			LeviathanCore::Platform::InitializeTiming();

			EngineRunning = true;

			PreMainLoopCallback.Call();
			MainLoop();
			PostMainLoopCallback.Call();

			LeviathanCore::Platform::DestroyDebugConsole();

			return 0;
		}

		void Exit()
		{
			EngineRunning = false;
		}
	}
}