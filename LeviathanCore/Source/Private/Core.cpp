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
			PreMainLoopCallback.Call();

			while (EngineRunning)
			{
				LeviathanCore::Platform::TickPlatform();
				TickCallback.Call(LeviathanCore::Platform::GetDeltaTimeInSeconds());
			}

			PostMainLoopCallback.Call();
		}

		int RunEngine()
		{
			LeviathanCore::Platform::CreateDebugConsole();

			if (!LeviathanCore::Platform::Initialize())
			{
				return 1;
			}

			// Create the runtime window.


			// Enter engine main loop.
			EngineRunning = true;

			MainLoop();

			return 0;
		}

		void Exit()
		{
			EngineRunning = false;
		}
	}
}