#pragma once

namespace LeviathanCore
{
	class InputKey;

	template<typename CallbackType>
	class Callback;

	namespace Platform
	{
		namespace Window
		{
			enum class PlatformWindowMode : unsigned char
			{
				Windowed = 0,
				Windowed_NoResize,
				Windowed_NoDragSize,
				Borderless
			};

			struct PlatformWindowDescription
			{
				int PositionX = 0;
				int PositionY = 0;
				int Width = 0;
				int Height = 0;
				std::string UniqueName = {};
				std::string Title = {};
				PlatformWindowMode Mode = PlatformWindowMode::Windowed;
				void* ParentWindowPlatformHandle = nullptr;
			};

			using PlatformWindowDestroyedCallbackType = void(*)();
			using PlatformWindowClosedCallbackType = void(*)();
			using PlatformWindowLostFocusCallbackType = void(*)();
			using PlatformWindowReceivedFocusCallbackType = void(*)();
			using PlatformWindowResizedCallbackType = void(*)(int /* resizedWidth */, int /* resizedHeight */);
			using PlatformWindowExitSizeMoveCallbackType = void(*)();
			using PlatformWindowEnterSizeMoveCallbackType = void(*)();
			using PlatformWindowRestoredCallbackType = void(*)();
			using PlatformWindowMinimizedCallbackType = void(*)();
			using PlatformWindowMaximizedCallbackType = void(*)();
			using PlatformWindowKeyboardInputCallbackType = void(*)(InputKey /* key */, bool /* repeatedKey */, float /* data */);
			using PlatformWindowMouseInputCallbackType = void(*)(InputKey /* key */, float /* data */);

			class PlatformWindow;

			PlatformWindow* CreatePlatformWindow();
			void DestroyPlatformWindow(PlatformWindow* const platformWindow);
			bool InitializePlatformWindow(PlatformWindow* const platformWindow, const PlatformWindowDescription& description);
			bool ShutdownPlatformWindow(PlatformWindow* const platformWindow);
			void* GetPlatformWindowPlatformHandle(PlatformWindow* const platformWindow);
			bool IsPlatformWindowMinimized(PlatformWindow* const platformWindow);
			bool EnterFullscreen(PlatformWindow* const platformWindow);
			bool ExitFullscreen(PlatformWindow* const platformWindow);
			bool IsPlatformWindowFullscreen(PlatformWindow* const platformWindow);
			bool GetPlatformWindowRenderAreaDimensions(PlatformWindow* const platformWindow, int& outWidth, int& outHeight);
			bool SetCursorPositionRelativeToPlatformWindow(PlatformWindow* const platformWindow, int x, int y);

			// Capture the platform cursor within the platform window's render area.
			bool CaptureCursor(PlatformWindow* const platformWindow);

			Callback<PlatformWindowDestroyedCallbackType>& GetPlatformWindowDestroyedCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowClosedCallbackType>& GetPlatformWindowClosedCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowLostFocusCallbackType>& GetPlatformWindowLostFocusCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowReceivedFocusCallbackType>& GetPlatformWindowReceivedFocusCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowResizedCallbackType>& GetPlatformWindowResizedCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowExitSizeMoveCallbackType>& GetPlatformWindowExitSizeMoveCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowEnterSizeMoveCallbackType>& GetPlatformWindowEnterSizeMoveCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowRestoredCallbackType>& GetPlatformWindowRestoredCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowMinimizedCallbackType>& GetPlatformWindowMinimizedCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowMaximizedCallbackType>& GetPlatformWindowMaximizedCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowKeyboardInputCallbackType>& GetPlatformWindowKeyboardInputCallback(PlatformWindow* const platformWindow);
			Callback<PlatformWindowMouseInputCallbackType>& GetPlatformWindowMouseInputCallback(PlatformWindow* const platformWindow);
		}
	}
}