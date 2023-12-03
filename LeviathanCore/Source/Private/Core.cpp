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

			if (!LeviathanCore::Platform::Initialize())
			{
				return 1;
			}

			EngineRunning = true;

			PreMainLoopCallback.Call();
			MainLoop();
			PostMainLoopCallback.Call();

			return 0;
		}

		void Exit()
		{
			EngineRunning = false;
		}
	}
}