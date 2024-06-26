#include "Core.h"
#include "Platform.h"
#include "PlatformWindow.h"
#include "Logging.h"
#include "InputKey.h"

#ifdef LEVIATHAN_WITH_TOOLS
#include "LeviathanTools.h"
#endif // LEVIATHAN_WITH_TOOLS.

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
		static Callback<PresentCallbackType> PresentCallback = {};

#ifdef LEVIATHAN_WITH_TOOLS
		static Callback<ImGuiRendererNewFrameCallbackType> ImGuiRendererNewFrameCallback = {};
		static Callback<ImGuiRenderCallbackType> ImGuiRenderCallback = {};
#endif // LEVIATHAN_WITH_TOOLS.

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

		static void OnPlatformWindowClosed()
		{
			Exit();
		}

		static void OnPlatformWindowResized(int newWidth, int newHeight)
		{
			RuntimeWindowResizedCallback.Call(newWidth, newHeight);
		}

		static void OnPlatformWindowMinimized()
		{
			RuntimeWindowMinimizedCallback.Call();
		}

		static void OnPlatformWindowMaximized()
		{
			RuntimeWindowMaximizedCallback.Call();
		}

		static void OnPlatformWindowRestored()
		{
			RuntimeWindowRestoredCallback.Call();
		}

		static void OnPlatformWindowMouseInput(LeviathanCore::InputKey key, float data)
		{
			RuntimeWindowMouseInputCallback.Call(key, data);
		}

		static void RegisterToRuntimeWindowCallbacks()
		{
			LeviathanCore::Platform::Window::GetPlatformWindowClosedCallback(RuntimeWindow).Register(&OnPlatformWindowClosed);
			LeviathanCore::Platform::Window::GetPlatformWindowResizedCallback(RuntimeWindow).Register(&OnPlatformWindowResized);
			LeviathanCore::Platform::Window::GetPlatformWindowMinimizedCallback(RuntimeWindow).Register(&OnPlatformWindowMinimized);
			LeviathanCore::Platform::Window::GetPlatformWindowMaximizedCallback(RuntimeWindow).Register(&OnPlatformWindowMaximized);
			LeviathanCore::Platform::Window::GetPlatformWindowRestoredCallback(RuntimeWindow).Register(&OnPlatformWindowRestored);
			LeviathanCore::Platform::Window::GetPlatformWindowMouseInputCallback(RuntimeWindow).Register(&OnPlatformWindowMouseInput);
		}

		static bool Cleanup()
		{
			CleanupCallback.Call();

			LeviathanCore::Platform::Window::GetPlatformWindowClosedCallback(RuntimeWindow).Deregister(&OnPlatformWindowClosed);
			LeviathanCore::Platform::Window::GetPlatformWindowResizedCallback(RuntimeWindow).Deregister(&OnPlatformWindowResized);
			LeviathanCore::Platform::Window::GetPlatformWindowMinimizedCallback(RuntimeWindow).Deregister(&OnPlatformWindowMinimized);
			LeviathanCore::Platform::Window::GetPlatformWindowMaximizedCallback(RuntimeWindow).Deregister(&OnPlatformWindowMaximized);
			LeviathanCore::Platform::Window::GetPlatformWindowRestoredCallback(RuntimeWindow).Deregister(&OnPlatformWindowRestored);
			LeviathanCore::Platform::Window::GetPlatformWindowMouseInputCallback(RuntimeWindow).Deregister(&OnPlatformWindowMouseInput);

#ifdef LEVIATHAN_WITH_TOOLS
			Platform::ImGuiPlatformShutdown();
			LeviathanTools::Shutdown();
#endif // LEVIATHAN_WITH_TOOLS.

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

		Callback<PresentCallbackType>& GetPresentCallback()
		{
			return PresentCallback;
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

#ifdef LEVIATHAN_WITH_TOOLS
					ImGuiRendererNewFrameCallback.Call();
					Platform::ImGuiPlatformNewFrame();
					ImGui::NewFrame();
					ImGuiRenderCallback.Call();
#endif // LEVIATHAN_WITH_TOOLS.

					PresentCallback.Call();
				}
			}

			PostMainLoopCallback.Call();
		}

#ifdef LEVIATHAN_WITH_TOOLS
		Callback<ImGuiRendererNewFrameCallbackType>& GetImGuiRendererNewFrameCallback()
		{
			return ImGuiRendererNewFrameCallback;
		}

		Callback<ImGuiRenderCallbackType>& GetImGuiRenderCallback()
		{
			return ImGuiRenderCallback;
		}
#endif // LEVIATHAN_WITH_TOOLS.

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

#ifdef LEVIATHAN_WITH_TOOLS
			// Initialize Leviathan tools module and platform backend implementation.
			if (!LeviathanTools::Initialize())
			{
				return false;
			}

			if (!Platform::ImGuiPlatformInitialize(GetRuntimeWindowPlatformHandle()))
			{
				return false;
			}
#endif // LEVIATHAN_WITH_TOOLS.

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

			if (!LeviathanCore::Platform::Shutdown())
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

		bool IsRuntimeWindowFullscreen()
		{
			return LeviathanCore::Platform::Window::IsPlatformWindowFullscreen(RuntimeWindow);
		}

		bool IsRuntimeWindowFocused()
		{
			return LeviathanCore::Platform::Window::IsPlatformWindowFocused(RuntimeWindow);
		}

		bool RuntimeWindowEnterFullscreen()
		{
			return LeviathanCore::Platform::Window::EnterFullscreen(RuntimeWindow);
		}

		bool RuntimeWindowExitFullscreen()
		{
			return LeviathanCore::Platform::Window::ExitFullscreen(RuntimeWindow);
		}

		bool SetCursorPosInRuntimeWindow(int x, int y)
		{
			return LeviathanCore::Platform::Window::SetCursorPositionRelativeToPlatformWindow(RuntimeWindow, x, y);
		}

		float GetDeltaSeconds()
		{
			return DeltaSeconds;
		}

		unsigned int GetPerfFPS()
		{
			return Fps;
		}

		float GetPerfMs()
		{
			return Ms;
		}
	}
}