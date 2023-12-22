#include "Core.h"
#include "Platform.h"
#include "PlatformWindow.h"

namespace LeviathanCore
{
	namespace Core
	{
		// Seconds elapsed between fixed ticks. FixedTick is called every SliceSeconds seconds.
		static constexpr float SliceSeconds = 0.1f; 
		// Timestep used in fixed tick call. Can be tweaked for a less or more precise simulation.
		static constexpr float FixedTimestep = 0.1f; 

		static LeviathanCore::Platform::Window::PlatformWindow* RuntimeWindow = {};
		static bool EngineRunning = false;
		static Callback<PreMainLoopCallbackType> PreMainLoopCallback = {};
		static Callback<PostMainLoopCallbackType> PostMainLoopCallback = {};
		static Callback<FixedTickCallbackType> FixedTickCallback = {};
		static Callback<PreTickCallbackType> PreTickCallback = {};
		static Callback<TickCallbackType> TickCallback = {};
		static Callback<PostTickCallbackType> PostTickCallback = {};
		static Callback<CleanupCallbackType> CleanupCallback = {};
		static Callback<RuntimeWindowResizedCallbackType> RuntimeWindowResizedCallback = {};

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
			LeviathanCore::Platform::Window::GetPlatformWindowClosedCallback(RuntimeWindow).Register(Exit);
			LeviathanCore::Platform::Window::GetPlatformWindowResizedCallback(RuntimeWindow).Register([](int newWidth, int newHeight)
				{
					RuntimeWindowResizedCallback.Call(newWidth, newHeight); 
				});
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

				const float deltaSeconds = LeviathanCore::Platform::GetDeltaTimeInSeconds();

				static float accumulator = 0.0;
				accumulator += deltaSeconds;

				PreTickCallback.Call();

				while (accumulator > SliceSeconds)
				{
					FixedTickCallback.Call(FixedTimestep);
					accumulator -= SliceSeconds;
				}

				TickCallback.Call(deltaSeconds);

				PostTickCallback.Call();
			}

			PostMainLoopCallback.Call();
		}

		int RunEngine()
		{
#ifndef LEVIATHAN_BUILD_CONFIG_MASTER
			LeviathanCore::Platform::CreateDebugConsole();
#endif // !LEVIATHAN_BUILD_CONFIG_MASTER

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

#ifndef LEVIATHAN_BUILD_CONFIG_MASTER
			LeviathanCore::Platform::DestroyDebugConsole();
#endif // !LEVIATHAN_BUILD_CONFIG_MASTER

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

		Callback<FixedTickCallbackType>& GetFixedTickCallback()
		{
			return FixedTickCallback;
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

		Callback<RuntimeWindowResizedCallbackType>& GetRuntimeWindowResizedCallback()
		{
			return RuntimeWindowResizedCallback;
		}
	}
}