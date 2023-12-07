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
		static Callback<PreTickCallbackType> PreTickCallback = {};
		static Callback<TickCallbackType> TickCallback = {};
		static Callback<PostTickCallbackType> PostTickCallback = {};
		static Callback<CleanupCallbackType> CleanupCallback = {};

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
			runtimeWindowDesc.UniqueName = "LeviathanEngineRuntimeWindow";
			runtimeWindowDesc.Mode = LeviathanCore::Platform::Window::PlatformWindowMode::Windowed_NoResize;
			return LeviathanCore::Platform::Window::InitializePlatformWindow(RuntimeWindow, runtimeWindowDesc);
		}

		static bool ShutdownAndDestroyRuntimeWindow()
		{
			if (!LeviathanCore::Platform::Window::ShutdownPlatformWindow(RuntimeWindow))
			{
				return false;
			}

			LeviathanCore::Platform::Window::DestroyPlatformWindow(RuntimeWindow);
			RuntimeWindow = nullptr;

			return true;
		}

		static void RegisterToRuntimeWindowCallbacks()
		{
			LeviathanCore::Platform::Window::GetPlatformWindowClosedCallback(RuntimeWindow).Register(OnRuntimeWindowClosed);
		}

		static bool Cleanup()
		{
			CleanupCallback.Call();

			return true;
		}

		void MainLoop()
		{
			PreMainLoopCallback.Call();

			while (EngineRunning)
			{
				LeviathanCore::Platform::TickPlatform();

				PreTickCallback.Call();
				TickCallback.Call(LeviathanCore::Platform::GetDeltaTimeInSeconds());
				PostTickCallback.Call();
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
			if (!Cleanup())
			{
				return 1;
			}

			// Destroy the runtime window.
			if (!ShutdownAndDestroyRuntimeWindow())
			{
				return false;
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

		Callback<PreTickCallbackType>& GetPreTickCallback()
		{
			return PreTickCallback;
		}

		Callback<TickCallbackType>& GetTickCallback()
		{
			return TickCallback;
		}

		Callback<PostTickCallbackType>& GetPostTickCallback()
		{
			return PostTickCallback;
		}

		Callback<CleanupCallbackType>& GetCleanupCallback()
		{
			return CleanupCallback;
		}
	}
}