#include "Core.h"
#include "Platform.h"
#include "PlatformWindow.h"

namespace LeviathanCore
{
	namespace Core
	{
		static LeviathanCore::Platform::Window::PlatformWindow* RuntimeWindow = {};
		static bool EngineRunning = false;
		static Callback<PreMainLoopCallbackType> PreMainLoopCallback = {};
		static Callback<PostMainLoopCallbackType> PostMainLoopCallback = {};
		static Callback<TickCallbackType> TickCallback = {};

		static void OnRuntimeWindowClosed()
		{
			Exit();
		}

		static bool CreateAndInitializeRuntimeWindow()
		{
			RuntimeWindow = LeviathanCore::Platform::Window::CreatePlatformWindow();

			LeviathanCore::Platform::Window::PlatformWindowDescription runtimeWindowDesc = {};
			runtimeWindowDesc.Width = 1280;
			runtimeWindowDesc.Height = 720;
			runtimeWindowDesc.Title = "Leviathan Engine";
			runtimeWindowDesc.UniqueName = "LevEngRuntimeWindow";
			runtimeWindowDesc.Mode = LeviathanCore::Platform::Window::PlatformWindowMode::Windowed_NoResize;
			return LeviathanCore::Platform::Window::InitializePlatformWindow(RuntimeWindow, runtimeWindowDesc);
		}

		static bool DestroyRuntimeWindow()
		{
			if (!LeviathanCore::Platform::Window::DestroyPlatformWindow(RuntimeWindow))
			{
				return false;
			}

			RuntimeWindow = nullptr;

			return true;
		}

		static void RegisterToRuntimeWindowCallbacks()
		{
			LeviathanCore::Platform::Window::GetPlatformWindowClosedCallback(RuntimeWindow).Register(OnRuntimeWindowClosed);
		}

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

			// Initialize platform layer.
			if (!LeviathanCore::Platform::Initialize())
			{
				return 1;
			}

			// Create the runtime window.
			if (!CreateAndInitializeRuntimeWindow())
			{
				return 1;
			}

			RegisterToRuntimeWindowCallbacks();

			// Enter engine main loop.
			EngineRunning = true;
			MainLoop();

			// Cleanup.
			if (!DestroyRuntimeWindow())
			{
				return 1;
			}

			return 0;
		}

		void Exit()
		{
			EngineRunning = false;
		}

		Callback<PreMainLoopCallbackType>& GetPreMainLoopCallback()
		{
			return PreMainLoopCallback;
		}

		Callback<PostMainLoopCallbackType>& GetPostMainLoopCallback()
		{
			return PostMainLoopCallback;
		}

		Callback<TickCallbackType>& GetTickCallback()
		{
			return TickCallback;
		}
	}
}