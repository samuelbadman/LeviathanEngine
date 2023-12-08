#include "XInputGamepad.h"

namespace LeviathanInput
{
	namespace PlatformInput
	{
		namespace XInputGamepad
		{
			// Callback objects.
			static LeviathanCore::Callback<XInputGamepadInputCallbackType> XInputGamepadInputCallback = {};
			static LeviathanCore::Callback<XInputGamepadConnectedCallbackType> XInputGamepadConnectedInputCallback = {};
			static LeviathanCore::Callback<XInputGamepadDisconnectedCallbackType> XInputGamepadDisconnectedInputCallback = {};


			void Initialize()
			{
			}

			void RefreshConnectedGameControllerFlags()
			{
			}

			void LogConnectedGameControllers()
			{
			}

			void RefreshConnectedGameControllerStates()
			{
			}

			void UpdatePreviousConnectedGameControllerStates()
			{
			}

			void DispatchMessagesForGameControllerKey(const LeviathanCore::InputKey::Keys key, const unsigned int gameControllerId)
			{
			}

			bool SetVibration(const unsigned short leftMotorSpeed, const unsigned short rightMotorSpeed, const unsigned int gameControllerId)
			{
				return false;
			}

			LeviathanCore::Callback<XInputGamepadInputCallbackType>& GetXInputGamepadInputCallback()
			{
				return XInputGamepadInputCallback;
			}

			LeviathanCore::Callback<XInputGamepadConnectedCallbackType>& GetXInputGamepadConnectedCallback()
			{
				return XInputGamepadConnectedInputCallback;
			}

			LeviathanCore::Callback<XInputGamepadDisconnectedCallbackType>& GetXInputGamepadDisconnectedCallback()
			{
				return XInputGamepadDisconnectedInputCallback;
			}

		}
	}
}