#pragma once

#include "PlatformWindow.h"
#include "Callback.h"
#include "InputKey.h"

namespace LeviathanCore
{
	namespace Platform
	{
		namespace Window
		{
			class PlatformWindow
			{
			private:
				// Game controller connection event callbacks are not exposed out from the platform layer as only the Win32 platform's message window can receive these events.
				// To register to these events use the callback in the main platform file.
				using PlatformWindowGameControllerConnectedCallbackType = void(*)();
				using PlatformWindowGameControllerDisconnectedCallbackType = void(*)();

				Callback<PlatformWindowDestroyedCallbackType> DestroyedCallback = {};
				Callback<PlatformWindowClosedCallbackType> ClosedCallback = {};
				Callback<PlatformWindowGameControllerConnectedCallbackType> GameControllerConnectedCallback = {};
				Callback<PlatformWindowGameControllerDisconnectedCallbackType> GameControllerDisconnectedCallback = {};
				Callback<PlatformWindowLostFocusCallbackType> LostFocusCallback = {};
				Callback<PlatformWindowReceivedFocusCallbackType> ReceivedFocusCallback = {};
				Callback<PlatformWindowResizedCallbackType> ResizedCallback = {};
				Callback<PlatformWindowExitSizeMoveCallbackType> ExitSizeMoveCallback = {};
				Callback<PlatformWindowEnterSizeMoveCallbackType> EnterSizeMoveCallback = {};
				Callback<PlatformWindowRestoredCallbackType> RestoredCallback = {};
				Callback<PlatformWindowMinimizedCallbackType> MinimizedCallback = {};
				Callback<PlatformWindowMaximizedCallbackType> MaximizedCallback = {};
				Callback<PlatformWindowKeyboardInputCallbackType> KeyboardInputCallback = {};
				Callback<PlatformWindowMouseInputCallbackType> MouseInputCallback = {};

				// Windows handle to the window instance.
				HWND Hwnd = nullptr;

				// Flags if the window is currently in a size move operation.
				bool InSizeMove = false;

				// The unique class name used to register the window class with the Windows platform. Used to unregister the window.
				std::string ClassName = {};

				int ClientAreaWidth = 0;
				int ClientAreaHeight = 0;

				bool Fullscreen = false;
				RECT RectPreLastFullscreen = {};
				DWORD WinStyle = 0;

			public:
				bool Initialize(std::string_view uniqueName, std::string_view windowTitle, DWORD style, int positionX,
					int positionY, int width, int height, HWND hWndParent, bool messageWindow);
				bool UnregisterWindowClassName();
				LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
				HWND GetHWnd() const { return Hwnd; }
				bool SetTitle(std::string_view newTitle);
				void Reset();
				bool IsMinimized() const;
				bool EnterFullscreen();
				bool ExitFullscreen();
				bool GetRenderAreaDimensions(int& outWidth, int& outHeight);
				inline int GetClientAreaWidth() const { return ClientAreaWidth; }
				inline int GetClientAreaHeight() const { return ClientAreaHeight; }
				bool SetCursorPosInWindow(int x, int y);
				inline bool IsFullscreen() const { return Fullscreen; }

				Callback<PlatformWindowDestroyedCallbackType>& GetDestroyedCallback() { return DestroyedCallback; }
				Callback<PlatformWindowClosedCallbackType>& GetClosedCallback() { return ClosedCallback; }
				Callback<PlatformWindowGameControllerConnectedCallbackType>& GetGameControllerConnectedCallback() { return GameControllerConnectedCallback; }
				Callback<PlatformWindowGameControllerDisconnectedCallbackType>& GetGameControllerDisconnectedCallback() { return GameControllerDisconnectedCallback; }
				Callback<PlatformWindowLostFocusCallbackType>& GetLostFocusCallback() { return LostFocusCallback; }
				Callback<PlatformWindowReceivedFocusCallbackType>& GetReceivedFocusCallback() { return ReceivedFocusCallback; }
				Callback<PlatformWindowResizedCallbackType>& GetResizedCallback() { return ResizedCallback; }
				Callback<PlatformWindowExitSizeMoveCallbackType>& GetExitSizeMoveCallback() { return ExitSizeMoveCallback; }
				Callback<PlatformWindowEnterSizeMoveCallbackType>& GetEnterSizeMoveCallback() { return EnterSizeMoveCallback; }
				Callback<PlatformWindowRestoredCallbackType>& GetRestoredCallback() { return RestoredCallback; }
				Callback<PlatformWindowMinimizedCallbackType>& GetMinimizedCallback() { return MinimizedCallback; }
				Callback<PlatformWindowMaximizedCallbackType>& GetMaximizedCallback() { return MaximizedCallback; }
				Callback<PlatformWindowKeyboardInputCallbackType>& GetKeyboardInputCallback() { return KeyboardInputCallback; }
				Callback<PlatformWindowMouseInputCallbackType>& GetMouseInputCallback() { return MouseInputCallback; }

			private:
				bool GetClientAreaDimensions(int& outWidth, int& outHeight);

				void WndProcDestroyed();
				void WndProcClosed();
				void WndProcGameControllerDisconnected();
				void WndProcGameControllerConnected();
				void WndProcLostFocus();
				void WndProcReceivedFocus();
				void WndProcResized(int resizedWidth, int resizedHeight);
				void WndProcExitSizeMove();
				void WndProcEnterSizeMove();
				void WndProcRestored();
				void WndProcMinimized();
				void WndProcMaximized();
				void WndProcKeyboardInput(const InputKey::Keys key, const bool repeatedKey, const float data);
				void WndProcMouseInput(const InputKey::Keys key, const float data);
			};
		}
	}
}