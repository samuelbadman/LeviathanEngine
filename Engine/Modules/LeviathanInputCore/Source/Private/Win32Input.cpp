#include "PlatformInput.h"
#include "Win32MouseState.h"
#include "Win32KeyboardState.h"
#include "Callback.h"
#include "XInputGamepad.h"

namespace LeviathanInputCore
{
	namespace PlatformInput
	{
		// Callback objects.
		static LeviathanCore::Callback<InputCallbackType> InputCallback = {};
		static LeviathanCore::Callback<GameControllerInputCallbackType> GameControllerInputCallback = {};
		static LeviathanCore::Callback<GameControllerConnectedCallbackType> GameControllerConnectedCallback = {};
		static LeviathanCore::Callback<GameControllerDisconnectedCallbackType> GameControllerDisconnectedCallback = {};

		// Handle to the mouse hook used to retrieve mouse hardware information independently of a window.
		static HHOOK MouseHook = nullptr;

		// Mouse state.
		static Win32MouseState PreviousMouseState = {};
		static Win32MouseState CurrentMouseState = {};

		// Mouse hook callback.
		static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
		{
			// Debug builds do not use low level mouse hook structures due to mouse unresponsiveness when debugging. See LeviathanInput::PlatformInput::Initialize.
//#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
			MOUSEHOOKSTRUCTEX* pMouseStruct = reinterpret_cast<MOUSEHOOKSTRUCTEX*>(lParam);
//#else
//			MSLLHOOKSTRUCT* pMouseStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
//#endif // LEVIATHAN_BUILD_CONFIG_DEBUG

			if (pMouseStruct != nullptr)
			{
				if (wParam == WM_MOUSEWHEEL)
				{
					if (HIWORD(pMouseStruct->mouseData) == 120)
					{
						CurrentMouseState.UpdateMouseWheel(LeviathanCore::InputKey::Keys::MouseWheelUpAxis, true);
					}
					else
					{
						CurrentMouseState.UpdateMouseWheel(LeviathanCore::InputKey::Keys::MouseWheelDownAxis, true);
					}
				}
				//if (wParam == WM_MBUTTONDOWN)
				// {
				//	MScrollBtn = 1;
				// }

				CurrentMouseState.UpdatePosition(pMouseStruct->pt.x, pMouseStruct->pt.y);
			}

			return CallNextHookEx(MouseHook, nCode, wParam, lParam);
		}

		// Returns the signed delta between the current and previous mouse position on the given axis.
		static long long GetMouseAxisPositionDelta(const long long currentAxisPosition, const long long previousAxisPosition)
		{
			return currentAxisPosition - previousAxisPosition;
		}

		// Dispatches callbacks for a keyboard key.
		static void DispatchCallbackForKeyboardKey(const LeviathanCore::InputKey::Keys key, const Win32KeyboardState& previousState, const Win32KeyboardState& currentState)
		{
			if (!LeviathanCore::InputKey::IsKeyboardKey(key))
			{
				return;
			}

			if (currentState.IsKeyPressed(key))
			{
				// Key down.
				if (previousState.IsKeyPressed(key))
				{
					InputCallback.Call(LeviathanCore::InputKey(key), true, 1.0f);
				}
				else
				{
					InputCallback.Call(LeviathanCore::InputKey(key), false, 1.0f);
				}
			}
			else
			{
				// Key up.
				if (previousState.IsKeyPressed(key))
				{
					InputCallback.Call(LeviathanCore::InputKey(key), false, 0.0f);
				}
			}
		}

		// Dispatches callbacks for a mouse key.
		static void DispatchCallbackForMouseKey(const LeviathanCore::InputKey::Keys key, const Win32MouseState& currMouseState, const Win32MouseState& prevMouseState)
		{
			switch (key)
			{
			case LeviathanCore::InputKey::Keys::MouseWheelUpAxis:
			{
				InputCallback.Call(LeviathanCore::InputKey(key), false, ((currMouseState.GetMouseWheel(LeviathanCore::InputKey::Keys::MouseWheelUpAxis)) ? 1.0f : 0.0f));
				break;
			}

			case LeviathanCore::InputKey::Keys::MouseWheelDownAxis:
			{
				InputCallback.Call(LeviathanCore::InputKey(key), false, ((currMouseState.GetMouseWheel(LeviathanCore::InputKey::Keys::MouseWheelDownAxis)) ? 1.0f : 0.0f));
				break;
			}

			case LeviathanCore::InputKey::Keys::LeftMouseButton:
			case LeviathanCore::InputKey::Keys::RightMouseButton:
			case LeviathanCore::InputKey::Keys::MiddleMouseButton:
			{
				if (currMouseState.IsButtonPressed(key))
				{
					// Key down.
					if (prevMouseState.IsButtonPressed(key))
					{
						// Key was down.
						InputCallback.Call(LeviathanCore::InputKey(key), true, 1.0f);
					}
					else
					{
						InputCallback.Call(LeviathanCore::InputKey(key), false, 1.0f);
					}
				}
				else
				{
					// Key up.
					if (prevMouseState.IsButtonPressed(key))
					{
						// Key was down.
						InputCallback.Call(LeviathanCore::InputKey(key), false, 0.0f);
					}
				}
				break;
			}

			case LeviathanCore::InputKey::Keys::MouseXAxis:
			{
				InputCallback.Call(LeviathanCore::InputKey(key), false, 
					static_cast<float>(GetMouseAxisPositionDelta(currMouseState.GetMouseXPosition(), prevMouseState.GetMouseXPosition())));
				break;
			}

			case LeviathanCore::InputKey::Keys::MouseYAxis:
			{
				InputCallback.Call(LeviathanCore::InputKey(key), false,
					static_cast<float>(GetMouseAxisPositionDelta(currMouseState.GetMouseYPosition(), prevMouseState.GetMouseYPosition())));
				break;
			}

			default:
			{
				break;
			}
			}
		}

		static void UpdatePreviousMouseState(Win32MouseState& currentState, Win32MouseState& previousState)
		{
			currentState.UpdateMouseWheel(LeviathanCore::InputKey::Keys::MouseWheelUpAxis, false);
			currentState.UpdateMouseWheel(LeviathanCore::InputKey::Keys::MouseWheelDownAxis, false);
			previousState = currentState;
		}

		static void OnPreTick()
		{
			XInputGamepad::RefreshConnectedGamepadStates();
		}

		static void OnPostTick()
		{
			UpdatePreviousMouseState(CurrentMouseState, PreviousMouseState);
			XInputGamepad::UpdatePreviousConnectedGamepadStates();
		}

		static void OnGameControllerConnectionEvent()
		{
			XInputGamepad::RefreshConnectedGamepadFlags();
		}

		static void OnXInputGamepadInput(LeviathanCore::InputKey key, bool isRepeatKey, float data, unsigned int gamepadId)
		{
			GetGameControllerInputCallback().Call(key, isRepeatKey, data, gamepadId);
		}

		static void OnXInputGamepadConnected(unsigned int gamepadId)
		{
			GetGameControllerConnectedCallback().Call(gamepadId);
		}

		static void OnXInputGamepadDisconnected(unsigned int gamepadId)
		{
			GetGameControllerDisconnectedCallback().Call(gamepadId);
		}

		BOOL IsKeyDownAsyncKeyState(const LeviathanCore::InputKey::Keys key)
		{
			std::underlying_type<LeviathanCore::InputKey::Keys>::type getAsyncKeyStateKey = ((key == LeviathanCore::InputKey::Keys::MiddleMouseButton) ?
				4
				:
				static_cast<std::underlying_type<LeviathanCore::InputKey::Keys>::type>(key));

			if (0x80000000 & GetAsyncKeyState(getAsyncKeyStateKey)) // Most significant bit is high (set).
			{
				return TRUE;										// Key is down.
			}
			else
			{
				return FALSE;
			}
		}

		bool Initialize()
		{
			PreviousMouseState = {};
			CurrentMouseState = {};

			// Hooking a low level (*_LL) mouse callback procedure causes the mouse to become unresponsive when breaking during debugging as the hooked 
			// callback procedure does not run as the program execution is paused. Windows will timeout from the hook procedure to handle mouse mesages
			// itself but the timeout duration makes the mouse unresponsive.
			int32_t mouseHook = 0;
			DWORD threadId = 0;

//#ifdef LEVIATHAN_BUILD_CONFIG_DEBUG
			mouseHook = WH_MOUSE;
			threadId = GetCurrentThreadId();
//#else
//			mouseHook = WH_MOUSE_LL;
//			threadId = 0;
//#endif // LEVIATHAN_BUILD_CONFIG_DEBUG

			MouseHook = SetWindowsHookEx(mouseHook, mouseProc, GetModuleHandle(nullptr), threadId);
			if (MouseHook == nullptr)
			{
				return false;
			}

			LeviathanCore::Core::GetPreTickCallback().Register(OnPreTick);
			LeviathanCore::Core::GetPostTickCallback().Register(OnPostTick);

			LeviathanCore::Platform::GetGameControllerConnectionEventCallback().Register(OnGameControllerConnectionEvent);

			LeviathanInputCore::PlatformInput::XInputGamepad::GetXInputGamepadInputCallback().Register(OnXInputGamepadInput);
			LeviathanInputCore::PlatformInput::XInputGamepad::GetXInputGamepadConnectedCallback().Register(OnXInputGamepadConnected);
			LeviathanInputCore::PlatformInput::XInputGamepad::GetXInputGamepadDisconnectedCallback().Register(OnXInputGamepadDisconnected);

			return true;
		}

		bool Shutdown()
		{
			if (UnhookWindowsHookEx(MouseHook) == 0)
			{
				return false;
			}

			MouseHook = nullptr;

			LeviathanCore::Core::GetPostTickCallback().Deregister(OnPostTick);

			LeviathanCore::Platform::GetGameControllerConnectionEventCallback().Deregister(OnGameControllerConnectionEvent);

			return true;
		}

		void DispatchCallbackForKey(const LeviathanCore::InputKey::Keys key)
		{
			if (LeviathanCore::InputKey::IsKeyboardKey(key))
			{
				static Win32KeyboardState previousKeyboardState = {};
				Win32KeyboardState currentKeyboardState = {};
				currentKeyboardState.UpdateKeyState(key);
				DispatchCallbackForKeyboardKey(key, previousKeyboardState, currentKeyboardState);
				previousKeyboardState = currentKeyboardState;
			}
			else if (LeviathanCore::InputKey::IsMouseKey(key))
			{
				if ((key == LeviathanCore::InputKey::Keys::LeftMouseButton) ||
					(key == LeviathanCore::InputKey::Keys::RightMouseButton) ||
					(key == LeviathanCore::InputKey::Keys::MiddleMouseButton))
				{
					CurrentMouseState.UpdateButtonState(key);
				}

				DispatchCallbackForMouseKey(key, CurrentMouseState, PreviousMouseState);
			}
		}

		void DispatchCallbackForGameControllerKey(const LeviathanCore::InputKey::Keys key, const unsigned int gameControllerId)
		{
			XInputGamepad::DispatchMessagesForGamepadKey(key, gameControllerId);
		}

		LeviathanCore::Callback<InputCallbackType>& GetInputCallback()
		{
			return InputCallback;
		}

		LeviathanCore::Callback<GameControllerInputCallbackType>& GetGameControllerInputCallback()
		{
			return GameControllerInputCallback;
		}

		LeviathanCore::Callback<GameControllerConnectedCallbackType>& GetGameControllerConnectedCallback()
		{
			return GameControllerConnectedCallback;
		}

		LeviathanCore::Callback<GameControllerDisconnectedCallbackType>& GetGameControllerDisconnectedCallback()
		{
			return GameControllerDisconnectedCallback;
		}
	}
}