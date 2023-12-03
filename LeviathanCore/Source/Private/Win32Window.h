#pragma once

#include "Callback.h"
#include "InputKey.h"

namespace LeviathanCore
{
	namespace Platform
	{
		class PlatformWindow
		{
		public:
			using DestroyedCallbackType = void(*)();
			using ClosedCallbackType = void(*)();
			using GameControllerConnectedCallbackType = void(*)();
			using GameControllerDisconnectedCallbackType = void(*)();
			using LostFocusCallbackType = void(*)();
			using ReceivedFocusCallbackType = void(*)();
			using ResizedCallbackType = void(*)(int /* resizedWidth */, int /* resizedHeight */);
			using ExitSizeMoveCallbackType = void(*)();
			using EnterSizeMoveCallbackType = void(*)();
			using RestoredCallbackType = void(*)();
			using MinimizedCallbackType = void(*)();
			using MaximizedCallbackType = void(*)();
			using KeyboardInputCallbackType = void(*)(InputKey /* key */, bool /* repeatedKey */, float /* data */);
			using MouseInputCallbackType = void(*)(InputKey /* key */, float /* data */);

			Callback<DestroyedCallbackType> DestroyedCallback = {};
			Callback<ClosedCallbackType> ClosedCallback = {};
			Callback<GameControllerConnectedCallbackType> GameControllerConnectedCallback = {};
			Callback<GameControllerDisconnectedCallbackType> GameControllerDisconnectedCallback = {};
			Callback<LostFocusCallbackType> LostFocusCallback = {};
			Callback<ReceivedFocusCallbackType> ReceivedFocusCallback = {};
			Callback<ResizedCallbackType> ResizedCallback = {};
			Callback<ExitSizeMoveCallbackType> ExitSizeMoveCallback = {};
			Callback<EnterSizeMoveCallbackType> EnterSizeMoveCallback = {};
			Callback<RestoredCallbackType> RestoredCallback = {};
			Callback<MinimizedCallbackType> MinimizedCallback = {};
			Callback<MaximizedCallbackType> MaximizedCallback = {};
			Callback<KeyboardInputCallbackType> KeyboardInputCallback = {};
			Callback<MouseInputCallbackType> MouseInputCallback = {};

		private:
			// Windows handle to the window instance.
			HWND Handle = nullptr;

			// Flags if the window is currently in a size move operation.
			bool InSizeMove = false;

			// The unique class name used to register the window class with the Windows platform. Used to unregister the window.
			std::string ClassName = {};

		public:
			bool Initialize(std::string_view uniqueName, std::string_view windowTitle, DWORD style, int positionX, 
				int positionY, int width, int height, HWND hWndParent, bool messageWindow);
			bool UnregisterWindowClassName();
			LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			HWND GetHWnd() const { return Handle; }

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