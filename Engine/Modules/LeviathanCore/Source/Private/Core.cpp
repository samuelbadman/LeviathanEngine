#include "Core.h"
#include "Platform.h"
#include "PlatformWindow.h"
#include "Logging.h"
#include "InputKey.h"

namespace LeviathanCore
{
	namespace Core
	{
		// Seconds elapsed between fixed ticks. FixedTick is called every SliceSeconds seconds.
		static constexpr float SliceSeconds = 0.1f;
		// Timestep used in fixed tick call. Can be tweaked for a less or more precise simulation.
		static constexpr float FixedTimestep = 0.1f;

		static bool EngineRunning = false;
		static LeviathanCore::Platform::Window::PlatformWindow* RuntimeWindow = {};

		static unsigned int Fps = 0;
		static float Ms = 0.0f;
		static float DeltaSeconds = 0.0f;

		static Callback<PreMainLoopCallbackType> PreMainLoopCallback = {};
		static Callback<PostMainLoopCallbackType> PostMainLoopCallback = {};
		static Callback<FixedTickCallbackType> FixedTickCallback = {};
		static Callback<PreTickCallbackType> PreTickCallback = {};
		static Callback<TickCallbackType> TickCallback = {};
		static Callback<PostTickCallbackType> PostTickCallback = {};
		static Callback<CleanupCallbackType> CleanupCallback = {};
		static Callback<RuntimeWindowResizedCallbackType> RuntimeWindowResizedCallback = {};
		static Callback<RuntimeWindowMinimizedCallbackType> RuntimeWindowMinimizedCallback = {};
		static Callback<RuntimeWindowMaximizedCallbackType> RuntimeWindowMaximizedCallback = {};
		static Callback<RuntimeWindowRestoredCallbackType> RuntimeWindowRestoredCallback = {};
		static Callback<RuntimeWindowMouseInputCallbackType> RuntimeWindowMouseInputCallback = {};
		static Callback<RenderCallbackType> RenderCallback = {};

		static bool CreateAndInitializeRuntimeWindow()
		{
			RuntimeWindow = LeviathanCore::Platform::Window::CreatePlatformWindow();

			LeviathanCore::Platform::Window::PlatformWindowDescription runtimeWindowDesc = {};
			runtimeWindowDesc.Width = 1280;
			runtimeWindowDesc.Height = 720;
			runtimeWindowDesc.Title = "Leviathan Engine";
			runtimeWindowDesc.UniqueName = "LeviathanEngineRuntimeWindow";
			runtimeWindowDesc.Mode = LeviathanCore::Platform::Window::PlatformWindowMode::Windowed;

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
			LeviathanCore::Platform::Window::GetPlatformWindowClosedCallback(RuntimeWindow).Register(&Exit);
			LeviathanCore::Platform::Window::GetPlatformWindowResizedCallback(RuntimeWindow).Register([](int newWidth, int newHeight)
				{ RuntimeWindowResizedCallback.Call(newWidth, newHeight); });
			LeviathanCore::Platform::Window::GetPlatformWindowMinimizedCallback(RuntimeWindow).Register([]() { RuntimeWindowMinimizedCallback.Call(); });
			LeviathanCore::Platform::Window::GetPlatformWindowMaximizedCallback(RuntimeWindow).Register([]() { RuntimeWindowMaximizedCallback.Call(); });
			LeviathanCore::Platform::Window::GetPlatformWindowRestoredCallback(RuntimeWindow).Register([]() { RuntimeWindowRestoredCallback.Call(); });
			LeviathanCore::Platform::Window::GetPlatformWindowMouseInputCallback(RuntimeWindow).Register([](LeviathanCore::InputKey key, float data)
				{ RuntimeWindowMouseInputCallback.Call(key, data); });
		}

		static bool Cleanup()
		{
			CleanupCallback.Call();

			return true;
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

		Callback<RuntimeWindowMinimizedCallbackType>& GetRuntimeWindowMinimizedCallback()
		{
			return RuntimeWindowMinimizedCallback;
		}

		Callback<RuntimeWindowMaximizedCallbackType>& GetRuntimeWindowMaximizedCallback()
		{
			return RuntimeWindowMaximizedCallback;
		}

		Callback<RuntimeWindowRestoredCallbackType>& GetRuntimeWindowRestoredCallback()
		{
			return RuntimeWindowRestoredCallback;
		}

		Callback<RuntimeWindowMouseInputCallbackType>& GetRuntimeWindowMouseInputCallback()
		{
			return RuntimeWindowMouseInputCallback;
		}

		Callback<RenderCallbackType>& GetRenderCallback()
		{
			return RenderCallback;
		}

		void MainLoop()
		{
			PreMainLoopCallback.Call();

			while (EngineRunning)
			{
				LeviathanCore::Platform::TickPlatform();

				// Calculate frame time and frames per second.
				DeltaSeconds = LeviathanCore::Platform::GetDeltaTimeInSeconds();

				const float AvgFps = (1.f / DeltaSeconds);
				Ms = 1.f / AvgFps;
				Fps = static_cast<unsigned int>(AvgFps);
				//LEVIATHAN_LOG("FPS: %d, MS: %f", Fps, Ms);

				// Don't update or render the frame if the runtime window is minimized.
				if (!LeviathanCore::Platform::Window::IsPlatformWindowMinimized(RuntimeWindow))
				{
					static float accumulator = 0.0;
					accumulator += DeltaSeconds;

					PreTickCallback.Call();

					while (accumulator > SliceSeconds)
					{
						FixedTickCallback.Call(FixedTimestep);
						accumulator -= SliceSeconds;
					}

					TickCallback.Call(DeltaSeconds);

					PostTickCallback.Call();

					RenderCallback.Call();
				}
			}

			PostMainLoopCallback.Call();
		}

		bool PreModuleInitialization()
		{
#ifndef LEVIATHAN_BUILD_CONFIG_MASTER
			LeviathanCore::Platform::CreateDebugConsole();
#endif // !LEVIATHAN_BUILD_CONFIG_MASTER

			// Create the runtime window.
			if (!CreateAndInitializeRuntimeWindow())
			{
				return false;
			}

			return true;
		}

		bool PostModuleInitialization()
		{
			return true;
		}

		int RunEngine()
		{
			// Initialize platform layer.
			if (!LeviathanCore::Platform::Initialize())
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

		void* GetRuntimeWindowPlatformHandle()
		{
			return LeviathanCore::Platform::Window::GetPlatformWindowPlatformHandle(RuntimeWindow);
		}

		bool GetRuntimeWindowRenderAreaDimensions(int& outWidth, int& outHeight)
		{
			return LeviathanCore::Platform::Window::GetPlatformWindowRenderAreaDimensions(RuntimeWindow, outWidth, outHeight);
		}

		bool RuntimeWindowCaptureCursor()
		{
			return LeviathanCore::Platform::Window::CaptureCursor(RuntimeWindow);
		}

		float GetDeltaSeconds()
		{
			return DeltaSeconds;
		}
	}
}